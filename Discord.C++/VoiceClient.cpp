#include "VoiceClient.h"

#include <errno.h>
#include <opus/opus.h>
#include <sodium.h>
#include <stdio.h>
#include <time.h>

#include <algorithm>
#include <chrono>
#include <queue>

#include "Exceptions.h"
#include "Logger.h"
#include "static.h"

const unsigned short FRAME_MILLIS = 20;
const unsigned short SAMPLE_RATE = 48000;
const unsigned short CHANNELS = 2;
const unsigned short FRAME_SIZE = (SAMPLE_RATE / 1000) * FRAME_MILLIS;
const unsigned int BITRATE = 131072;

#define MAX_PACKET_SIZE FRAME_SIZE * 8

using namespace std;
using namespace web::json;
using namespace utility;
using namespace web::websockets::client;
using namespace boost::asio::ip;

DiscordCPP::udp_client::udp_client() {}

DiscordCPP::udp_client::udp_client(const string_t& ip, const int port) {
    _log = Logger("discord.VoiceClient.udp_client");

    udp::resolver resolver(_io_service);
    udp::resolver::query query(udp::v4(), conversions::to_utf8string(ip),
                               to_string(port));

    _remote = *resolver.resolve(query);

    _socket = new udp::socket(_io_service);

    _socket->open(udp::v4());
}

DiscordCPP::udp_client::~udp_client() {
    _log.debug("~udp_client");
    _socket->close();
    delete _socket;
}

void DiscordCPP::udp_client::send(const string& msg) {
    try {
        _socket->send_to(boost::asio::buffer(msg, msg.size()), _remote);
    } catch (exception& e) {
        _log.error("Cannot send message: " + string(e.what()));
    }
}

string DiscordCPP::udp_client::receive() {
    size_t len = 0;

    try {
        udp::endpoint sender_enpoint;
        len = _socket->receive_from(boost::asio::buffer(_recv_buffer),
                                    sender_enpoint);
    } catch (exception& e) {
        _log.error("Cannot receive message: " + string(e.what()));
    }

    return string(_recv_buffer.begin(), _recv_buffer.begin() + len);
}

pplx::task<void> DiscordCPP::VoiceClient::connect_voice_udp() {
    return pplx::create_task([this] {
        _udp = new udp_client(_server_ip, _server_port);

        _log.info("performing IP Discovery");

        string msg;
        msg.resize(74, '\0');
        // Type: 0x1 for request, 0x2 for response
        msg[0] = 0x1 >> 8;
        msg[1] = 0x1;
        // Message length excluding Type and Length fields (always 70)
        msg[2] = 70 >> 8;
        msg[3] = 70;
        // SSRC
        msg[4] = _ssrc >> 24;
        msg[5] = _ssrc >> 16;
        msg[6] = _ssrc >> 8;
        msg[7] = _ssrc;

        _udp->send(msg);

        string recv_msg = _udp->receive();

        string my_ip = recv_msg.substr(8, 20);
        for (unsigned int i = 0; i < my_ip.size(); i++) {
            if (my_ip[i] == 0) {
                my_ip.resize(i, '\0');
                break;
            }
        }

        unsigned short my_port = (recv_msg[72] << 8) | (recv_msg[73]);
        my_port = (my_port >> 8) | (my_port << 8);

        _my_ip = conversions::to_string_t(my_ip);
        _my_port = my_port;

        _log.info("found own IP and port: " + my_ip + ":" + to_string(my_port));
    });
}

pplx::task<void> DiscordCPP::VoiceClient::select_protocol() {
    return pplx::create_task([this] {
        value payload;
        payload[U("op")] = value(1);
        payload[U("d")][U("protocol")] = value(U("udp"));
        payload[U("d")][U("data")][U("address")] = value(_my_ip);
        payload[U("d")][U("data")][U("port")] = value(_my_port);
        payload[U("d")][U("data")][U("mode")] = value(U("xsalsa20_poly1305"));

        _voice_ws->send(payload)
            .then([this] {
                _log.debug("Opcode 1 Select Protocol Payload has been sent");
            })
            .wait();
    });
}

pplx::task<void> DiscordCPP::VoiceClient::load_session_description(
    const value& data) {
    return pplx::create_task([this, data] {
        _mode = data.at(U("mode")).as_string();
        web::json::array tmp = data.at(U("secret_key")).as_array();
        for (unsigned int i = 0; i < tmp.size(); i++) {
            _secret_key.push_back((unsigned char)tmp[i].as_integer());
        }
    });
}

pplx::task<void> DiscordCPP::VoiceClient::speak(bool speak) {
    return pplx::create_task([this, speak] {
        value payload;
        payload[U("op")] = value(5);
        payload[U("d")][U("speaking")] = value(speak);
        payload[U("d")][U("delay")] = value(0);
        payload[U("d")][U("ssrc")] = value(_ssrc);

        _voice_ws->send(payload)
            .then([this] {
                _log.debug("Opcode 5 Speaking Payload has been sent");
            })
            .wait();
    });
}

DiscordCPP::VoiceClient::VoiceClient(MainGateway** main_ws,
                                     const string_t& voice_token,
                                     const string_t& endpoint,
                                     const string_t& session_id,
                                     const string_t& guild_id,
                                     const string_t& channel_id,
                                     const string_t& user_id) {
    _guild_id = guild_id;
    _channel_id = channel_id;

    _log = Logger("Discord.VoiceClient");
    _log.info("connecting to endpoint " +
              conversions::to_utf8string(_endpoint));

    _main_ws = main_ws;
    _voice_ws = new VoiceGateway(conversions::to_utf8string(voice_token),
                                 conversions::to_utf8string(session_id),
                                 conversions::to_utf8string(guild_id),
                                 conversions::to_utf8string(user_id));

    _voice_ws->set_message_handler([this](value data) {
        switch (data.at(U("op")).as_integer()) {
            case 2:
                _ssrc = data[U("d")][U("ssrc")].as_integer();
                _server_ip = data[U("d")][U("ip")].as_string();
                _server_port = data[U("d")][U("port")].as_integer();
                connect_voice_udp().then([this] { select_protocol(); });
                break;
            case 4:
                load_session_description(data[U("d")]).then([this] {
                    _log.debug("mode: " + conversions::to_utf8string(_mode));
                    _log.info("handshake complete. voice connection ready.");
                    _ready = true;
                    speak();
                });
                break;
            case 6:
            case 8:
                break;
            default:
                _log.debug(conversions::to_utf8string(data.serialize()));
                break;
        }
    });

    _voice_ws->connect(conversions::to_utf8string(endpoint));

    while (_ready == false) {
        waitFor(chrono::milliseconds(10)).wait();
    }
}

DiscordCPP::VoiceClient::VoiceClient() {}

DiscordCPP::VoiceClient::~VoiceClient() {
    _log.debug("~VoiceClient");
    disconnect().wait();
    _voice_ws->close().wait();
    delete _voice_ws;
    delete _udp;
}

pplx::task<void> DiscordCPP::VoiceClient::disconnect() {
    return pplx::create_task([this] {
        value payload = value();
        payload[U("op")] = value(4);
        payload[U("d")][U("guild_id")] = value(_guild_id);
        payload[U("d")][U("channel_id")] = value::null();
        payload[U("d")][U("self_mute")] = value(true);
        payload[U("d")][U("self_deaf")] = value(true);

        (*_main_ws)
            ->send(payload)
            .then([this] {
                _log.debug(
                    "Payload with Opcode 4 (Gateway Voice State Update) has "
                    "been sent");
            })
            .wait();
    });
}

pplx::task<void> DiscordCPP::VoiceClient::play(AudioSource* source) {
    return pplx::create_task([this, source] {
        _log.debug("creating opus encoder");
        int error;
        OpusEncoder* encoder = opus_encoder_create(
            SAMPLE_RATE, CHANNELS, OPUS_APPLICATION_AUDIO, &error);
        if (error < 0) {
            throw OpusError("failed to create opus encoder: " +
                                string(opus_strerror(error)),
                            error);
        }

        opus_encoder_ctl(encoder, OPUS_SET_BITRATE(BITRATE));
        opus_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(1));
        opus_encoder_ctl(encoder, OPUS_SET_PACKET_LOSS_PERC(15));

        _log.debug("initialising libsodium");
        if (sodium_init() == -1) {
            throw ClientException("libsodium initialisation failed");
        }

        int num_opus_bytes;
        unsigned char* pcm_data = new unsigned char[FRAME_SIZE * CHANNELS * 2];
        opus_int16* in_data;
        vector<unsigned char> opus_data(MAX_PACKET_SIZE);

        _log.debug("starting loop");

        auto next_time = chrono::high_resolution_clock::now();

        while (1) {
            if (source->read((char*)pcm_data, FRAME_SIZE * CHANNELS * 2) !=
                true)
                break;

            in_data = reinterpret_cast<opus_int16*>(pcm_data);

            num_opus_bytes = opus_encode(encoder, in_data, FRAME_SIZE,
                                         opus_data.data(), MAX_PACKET_SIZE);
            if (num_opus_bytes <= 0) {
                throw OpusError("failed to encode frame: " +
                                    string(opus_strerror(num_opus_bytes)),
                                num_opus_bytes);
            }

            opus_data.resize(num_opus_bytes);

            string packet;
            packet.resize(12 + opus_data.size() + crypto_secretbox_MACBYTES,
                          '\0');

            packet[0] = 0x80;  // Type
            packet[1] = 0x78;  // Version

            packet[2] = _sequence >> 8;  // Sequence
            packet[3] = (unsigned char)_sequence;

            packet[4] = _timestamp >> 24;  // Timestamp
            packet[5] = _timestamp >> 16;
            packet[6] = _timestamp >> 8;
            packet[7] = _timestamp;

            packet[8] = (unsigned char)(_ssrc >> 24);  // SSRC
            packet[9] = (unsigned char)(_ssrc >> 16);
            packet[10] = (unsigned char)(_ssrc >> 8);
            packet[11] = (unsigned char)_ssrc;

            _sequence++;
            _timestamp += SAMPLE_RATE / 1000 * FRAME_MILLIS;

            string nonce;
            nonce.resize(crypto_secretbox_NONCEBYTES, '\0');
            for (int i = 0; i < 12; i++) {
                nonce[i] = packet[i];
            }

            crypto_secretbox_easy((unsigned char*)packet.data() + 12,
                                  opus_data.data(), opus_data.size(),
                                  (unsigned char*)nonce.data(),
                                  _secret_key.data());

            _udp->send(packet);

            next_time += chrono::milliseconds(FRAME_MILLIS);
            chrono::microseconds delay =
                std::max(chrono::microseconds(0),
                         chrono::duration_cast<chrono::microseconds>(
                             next_time - chrono::high_resolution_clock::now()));

            waitFor(delay).wait();
        }

        opus_encoder_destroy(encoder);
        delete[] pcm_data;

        _log.debug("finished playing audio");
    });
}
