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

using namespace boost::asio::ip;

DiscordCPP::udp_client::udp_client(const std::string& ip, const int port) {
    _log = Logger("discord.VoiceClient.udp_client");

    udp::resolver resolver(_io_service);
    udp::resolver::query query(udp::v4(), ip, std::to_string(port));

    _remote = *resolver.resolve(query);

    _socket = std::make_unique<udp::socket>(_io_service);

    _socket->open(udp::v4());
}

DiscordCPP::udp_client::~udp_client() {
    _log.debug("~udp_client");
    _socket->close();
}

void DiscordCPP::udp_client::send(const std::string& msg) {
    try {
        _socket->send_to(boost::asio::buffer(msg, msg.size()), _remote);
    } catch (std::exception& e) {
        _log.error("Cannot send message: " + std::string(e.what()));
    }
}

std::string DiscordCPP::udp_client::receive() {
    size_t len = 0;

    try {
        udp::endpoint sender_enpoint;
        len = _socket->receive_from(boost::asio::buffer(_recv_buffer),
                                    sender_enpoint);
    } catch (std::exception& e) {
        _log.error("Cannot receive message: " + std::string(e.what()));
    }

    return std::string(_recv_buffer.begin(), _recv_buffer.begin() + len);
}

std::shared_future<void> DiscordCPP::VoiceClient::connect_voice_udp() {
    return threadpool->execute([this] {
        _udp = std::make_unique<udp_client>(_server_ip, _server_port);

        _log.info("performing IP Discovery");

        std::string msg;
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

        std::string recv_msg = _udp->receive();

        std::string my_ip = recv_msg.substr(8, 20);
        for (unsigned int i = 0; i < my_ip.size(); i++) {
            if (my_ip[i] == 0) {
                my_ip.resize(i, '\0');
                break;
            }
        }

        unsigned short my_port = (recv_msg[72] << 8) | (recv_msg[73]);
        my_port = (my_port >> 8) | (my_port << 8);

        _my_ip = my_ip;
        _my_port = my_port;

        _log.info("found own IP and port: " + my_ip + ":" + std::to_string(my_port));
    });
}

std::shared_future<void> DiscordCPP::VoiceClient::select_protocol() {
    return threadpool->execute([this] {
        json payload = {
            {"op", 1},
            {"d", {
                      {"protocol", "udp"},  //
                      {"data", {
                                   //
                                   {"address", _my_ip},           //
                                   {"port", _my_port},            //
                                   {"mode", "xsalsa20_poly1305"}  //
                               }}                                 //
                  }}                                              //
        };

        _voice_ws->send(payload).get();
        _log.debug("Opcode 1 Select Protocol Payload has been sent");
    });
}

std::shared_future<void> DiscordCPP::VoiceClient::load_session_description(const json& data) {
    return threadpool->execute([this, data] {
        _mode = data["mode"].get<std::string>();
        for (json k : data["secret_key"]) {
            _secret_key.push_back(k.get<unsigned char>());
        }
    });
}

std::shared_future<void> DiscordCPP::VoiceClient::speak(bool speak) {
    return threadpool->execute([this, speak] {
        json payload = {
            {"op", 5},
            {
                "d", {
                         {"speaking", speak},  //
                         {"delay", 0},         //
                         {"ssrc", _ssrc}       //
                     }                         //
            }                                  //
        };

        _voice_ws->send(payload).get();
        _log.debug("Opcode 5 Speaking Payload has been sent");
    });
}

DiscordCPP::VoiceClient::VoiceClient(std::shared_ptr<MainGateway> main_ws,
                                     const std::string& voice_token,
                                     const std::string& endpoint,
                                     const std::string& session_id,
                                     const std::string& guild_id,
                                     const std::string& channel_id,
                                     const std::string& user_id) {
    threadpool = std::make_shared<Threadpool>(4);
    _guild_id = guild_id;
    _channel_id = channel_id;

    _log = Logger("Discord.VoiceClient");
    _log.info("connecting to endpoint " + _endpoint);

    _main_ws = main_ws;
    _voice_ws = std::make_unique<VoiceGateway>(voice_token, session_id, guild_id, user_id, threadpool);

    _voice_ws->set_message_handler([this](json data) {
        std::shared_future<void> f;
        switch (data["op"].get<int>()) {
            case 2:
                _ssrc = data["d"]["ssrc"].get<int>();
                _server_ip = data["d"]["ip"].get<std::string>();
                _server_port = data["d"]["port"].get<int>();
                f = connect_voice_udp();
                threadpool->then(f, [this]() { select_protocol(); });
                break;
            case 4:
                f = load_session_description(data["d"]);
                threadpool->then(f, [this]() {
                    _log.debug("mode: " + _mode);
                    _log.info("handshake complete. voice connection ready.");
                    _ready = true;
                    speak();
                });
                break;
            case 6:
            case 8:
                break;
            default:
                _log.debug(data.dump());
                break;
        }
    });

    _voice_ws->connect(endpoint);

    while (_ready == false) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

DiscordCPP::VoiceClient::~VoiceClient() {
    _log.debug("~VoiceClient");
    _ready = false;
    disconnect().get();
    _voice_ws->close().get();
}

std::shared_future<void> DiscordCPP::VoiceClient::disconnect() {
    if (!_ready) {
        std::promise<void> promise;
        std::shared_future<void> future(promise.get_future());
        promise.set_value();
        return future;
    }

    json payload = {
        {"op", 4},  //
        {"d", {
                  //
                  {"guild_id", _guild_id},  //
                  {"channel_id", nullptr},  //
                  {"self_mute", true},      //
                  {"self_deaf", true}       //
              }}                            //
    };
    std::shared_future<void> main_future = _main_ws->send(payload);
    std::shared_future<void> voice_future = _voice_ws->close();

    return threadpool->execute([this, voice_future, main_future] {
        _ready = false;

        voice_future.get();
        main_future.get();

        _log.debug("Payload with Opcode 4 (Gateway Voice State Update) has been sent");
    });
}

std::shared_future<void> DiscordCPP::VoiceClient::play(AudioSource* source) {
    return threadpool->execute([this, source] {
        _log.debug("creating opus encoder");
        int error;
        OpusEncoder* encoder = opus_encoder_create(
            SAMPLE_RATE, CHANNELS, OPUS_APPLICATION_AUDIO, &error);
        if (error < 0) {
            throw OpusError("failed to create opus encoder: " +
                                std::string(opus_strerror(error)),
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
        std::vector<unsigned char> opus_data(MAX_PACKET_SIZE);

        _log.debug("starting loop");

        auto next_time = std::chrono::high_resolution_clock::now();

        _playing = true;

        while (_ready) {
            if (source->read((char*)pcm_data, FRAME_SIZE * CHANNELS * 2) !=
                true)
                break;

            in_data = reinterpret_cast<opus_int16*>(pcm_data);

            num_opus_bytes = opus_encode(encoder, in_data, FRAME_SIZE,
                                         opus_data.data(), MAX_PACKET_SIZE);
            if (num_opus_bytes <= 0) {
                throw OpusError("failed to encode frame: " +
                                    std::string(opus_strerror(num_opus_bytes)),
                                num_opus_bytes);
            }

            opus_data.resize(num_opus_bytes);

            std::string packet;
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

            std::string nonce;
            nonce.resize(crypto_secretbox_NONCEBYTES, '\0');
            for (int i = 0; i < 12; i++) {
                nonce[i] = packet[i];
            }

            crypto_secretbox_easy((unsigned char*)packet.data() + 12,
                                  opus_data.data(), opus_data.size(),
                                  (unsigned char*)nonce.data(),
                                  _secret_key.data());

            _udp->send(packet);

            next_time += std::chrono::milliseconds(FRAME_MILLIS);
            std::this_thread::sleep_until(next_time);
        }

        _playing = false;

        opus_encoder_destroy(encoder);
        delete[] pcm_data;

        _log.debug("finished playing audio");
    });
}
