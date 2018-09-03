#include "VoiceClient.h"
#include "static.h"
#include "Logger.h"

#include <chrono>
#include <thread>
#include <time.h>
#include <stdio.h>
#include <errno.h>

#include <sodium.h>
#include <opus/opus.h>

using namespace std;
using namespace web::json;
using namespace utility;
using namespace web::websockets::client;
using namespace boost::asio::ip;

/*
void DiscordCPP::udp_client::wait_for_receive() {
	//_socket->async_receive_from(boost::asio::buffer(_recv_buffer), 
	//	_remote, 
	//	boost::bind(&udp_client::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void DiscordCPP::udp_client::handle_receive(boost::system::error_code &error, size_t bytes_tranferred) {
	if (error) {
		Logger("discord.VoiceClient.udp_client.handle_receive").error("Receive failed: " + error.message());
	}
	else {
		Logger("discord.VoiceClient.udp_client.handle_receive").debug(string(_recv_buffer.begin(), _recv_buffer.begin() + bytes_tranferred));
	}
	wait_for_receive();
}
*/
DiscordCPP::udp_client::udp_client() : _io_service(boost::asio::io_service()) {

}

DiscordCPP::udp_client::udp_client(string_t ip, int port) : _io_service(boost::asio::io_service()) {
	_log = Logger("discord.VoiceClient.udp_client");

	udp::resolver resolver(_io_service);
	udp::resolver::query query(udp::v4(), conversions::to_utf8string(ip), to_string(port));

	_remote = *resolver.resolve(query);

	_socket = new udp::socket(_io_service);

	_socket->open(udp::v4());
}

DiscordCPP::udp_client::udp_client(udp_client & old) : _io_service(old._io_service) {
	this->_log = old._log;
	this->_recv_buffer = old._recv_buffer;
	this->_socket = old._socket;
	this->_remote = old._remote;
}

DiscordCPP::udp_client::~udp_client() {
	_log.debug("~udp_client");
	_socket->close();
}

void DiscordCPP::udp_client::send(const string &msg) {
	try {
		_socket->send_to(boost::asio::buffer(msg, msg.size()), _remote);
	}
	catch (exception &e) {
		_log.error("Cannot send message: " + string(e.what()));
	}
}

string DiscordCPP::udp_client::receive() {
	size_t len = 0;
	
	try {
		udp::endpoint sender_enpoint;
		len = _socket->receive_from(boost::asio::buffer(_recv_buffer), sender_enpoint);
	}
	catch (exception &e) {
		_log.error("Cannot receive message: " + string(e.what()));
	}

	return string(_recv_buffer.begin(), _recv_buffer.begin() + len);
}

pplx::task<void> DiscordCPP::VoiceClient::identify() {
	return pplx::create_task([this] {
		value out_payload;
		websocket_outgoing_message out;

		out_payload[U("op")] = value(0);
		out_payload[U("d")][U("server_id")] = value(_guild_id);
		out_payload[U("d")][U("user_id")] = value(_user_id);
		out_payload[U("d")][U("session_id")] = value(_session_id);
		out_payload[U("d")][U("token")] = value(_voice_token);

		out.set_utf8_message(conversions::to_utf8string(out_payload.serialize()));
		_voice_ws.send(out).then([this] {
			Logger("Discord.VoiceClient").info("Identify payload has been sent");
		}).wait();
	});
}

pplx::task<void> DiscordCPP::VoiceClient::create_heartbeat_task() {
	return pplx::create_task([this] {
		while (_heartbeat_interval == 0)
			this_thread::sleep_for(chrono::milliseconds(50));

		while (_keepalive) {
			try {
				websocket_outgoing_message heartbeat_msg = websocket_outgoing_message();
				heartbeat_msg.set_utf8_message("{\"op\": 3, \"d\": " + to_string(time(NULL)) + "}");
				_voice_ws.send(heartbeat_msg).then([this] {
					_log.debug("Heartbeat message has been sent");
				}).wait();
			}
			catch (websocket_exception &e) {
				_log.error("Cannot send heartbeat message: " + string(e.what()) + " (" + to_string(e.error_code().value()) + ": " + e.error_code().message());
			}
			catch (exception &e) {
				_log.error("Cannot send heartbeat message: " + string(e.what()));
			}

			this_thread::sleep_for(chrono::milliseconds(_heartbeat_interval));
		}
	});
}

pplx::task<void> DiscordCPP::VoiceClient::connect_voice_udp() {
	return pplx::create_task([this] {
		_udp = new udp_client(_server_ip, _server_port);

		_log.info("performing IP Discovery");

		string msg;
		msg.resize(70, '\0');
		for (int i = 0; i < 4; i++) {
			msg[3 - i] = (_ssrc >> (i * 8));
		}
		_udp->send(msg);

		string recv_msg = _udp->receive();
		//_log.debug("Received message(length: " + to_string(msg.size()) + "): " + msg);

		string my_ip = recv_msg.substr(4, 16);
		for (int i = 0; i < my_ip.size(); i++) {
			if (my_ip[i] == 0) {
				my_ip.resize(i, '\0');
				break;
			}
		}

		_log.debug("my_ip(str.length: " + to_string(my_ip.size()) + "): " + my_ip);

		unsigned short my_port = (recv_msg[68] << 8) | (recv_msg[69]);
		_log.debug("my_port: " + to_string(my_port));

		my_port = (my_port >> 8) | (my_port << 8);
		_log.debug("my_port: " + to_string(my_port));

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

		websocket_outgoing_message msg;
		msg.set_utf8_message(conversions::to_utf8string(payload.serialize()));

		_voice_ws.send(msg).then([this] {
			_log.debug("Opcode 1 Select Protocol Payload has been sent");
		}).wait();
	});
}

pplx::task<void> DiscordCPP::VoiceClient::load_session_description(value data) {
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

		websocket_outgoing_message msg;
		msg.set_utf8_message(conversions::to_utf8string(payload.serialize()));

		_voice_ws.send(msg).then([this] {
			_log.debug("Opcode 5 Speaking Payload has been sent");
		}).wait();
	});
}

DiscordCPP::VoiceClient::VoiceClient(websocket_callback_client **main_ws, string_t voice_token, string_t endpoint, string_t session_id, string_t guild_id, string_t channel_id, string_t user_id) {
	_voice_token = voice_token;
	_endpoint = endpoint;
	_session_id = session_id;
	_guild_id = guild_id;
	_channel_id = channel_id;
	_user_id = user_id;

	_log = Logger("Discord.VoiceClient");
	_log.info("connecting to endpoint " + conversions::to_utf8string(_endpoint));

	_main_ws = main_ws;
	_voice_ws = websocket_callback_client();

	_voice_ws.set_close_handler([this](websocket_close_status close_status, string_t reason, error_code error) {
		_log.warning("websocket closed with status: " + to_string((int)close_status) + ": " + conversions::to_utf8string(reason) + " (" + to_string(error.value()) + ": " + error.message() + ")");
	});

	_voice_ws.set_message_handler([this](websocket_incoming_message msg) {
		string msg_string = msg.extract_string().get();
		value data = value::parse(conversions::to_string_t(msg_string));

		switch (data.at(U("op")).as_integer()) {
		case 2:
			_ssrc = data[U("d")][U("ssrc")].as_integer();
			_server_ip = data[U("d")][U("ip")].as_string();
			_server_port = data[U("d")][U("port")].as_integer();
			connect_voice_udp().then([this] {
				select_protocol();
			});
			break;
		case 4:
			load_session_description(data[U("d")]).then([this] {
				_log.debug("mode: " + conversions::to_utf8string(_mode));
				_log.info("handshake complete. voice connection ready.");
				_ready = true;
				speak();
				//pplx::create_task([this] {
				//	while (_keepalive) {
				//		string msg = _udp->receive();
				//		_log.debug("Received message(length: " + to_string(msg.size()) + "): " + msg);
				//	}
				//});
			});
			break;
		case 6:
			_log.debug("received heartbeat ack");
			break;
		case 8:
			_heartbeat_interval = (int)(data[U("d")][U("heartbeat_interval")].as_integer() * 0.75);
			identify();
			break;
		default:
			_log.debug(msg_string);
			break;
		}
	});

	_voice_ws.connect(_endpoint).then([this] {
		_heartbeat_task = create_heartbeat_task();
		_log.info("connected to voice websocket");
	});

	while (_ready == false)	{
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}

DiscordCPP::VoiceClient::VoiceClient(const VoiceClient & old) {
	this->_channel_id = old._channel_id;
	this->_guild_id = old._guild_id;
	this->_session_id = old._session_id;
	this->_user_id = old._user_id;
	this->_heartbeat_interval = old._heartbeat_interval;
	this->_server_ip = old._server_ip;
	this->_server_port = old._server_port;
	this->_endpoint = old._endpoint;
	this->_voice_ws = old._voice_ws;
	this->_voice_token = old._voice_token;
	this->_log = old._log;
	this->_ready = old._ready;
	this->_ssrc = old._ssrc;
	this->_udp = old._udp;
	this->_my_ip = old._my_ip;
	this->_my_port = old._my_port;
	this->_secret_key = old._secret_key;
}

DiscordCPP::VoiceClient::VoiceClient() {

}

DiscordCPP::VoiceClient::~VoiceClient() {
	_log.debug("~VoiceClient");
	disconnect().wait();
	_keepalive = false;
	_voice_ws.close(websocket_close_status::normal, U("class VoiceClient was destroyed"));
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

		websocket_outgoing_message msg;
		msg.set_utf8_message(conversions::to_utf8string(payload.serialize()));

		(*_main_ws)->send(msg).then([this] {
			_log.debug("Payload with Opcode 4 (Gateway Voice State Update) has been sent");
		}).wait();
	});
}

#define FRAME_SIZE 960
#define SAMPLE_RATE 48000
#define CHANNELS 2
#define APPLICATION OPUS_APPLICATION_AUDIO
#define BITRATE 64000

#define MAX_FRAME_SIZE 6*960
//#define MAX_PACKET_SIZE (3*1276)
#define MAX_PACKET_SIZE 8192

template <typename T,
	int N = sizeof(T) * 8,
	typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	inline T swapEndianness(T value)
{
	T result = 0;

	for (int i = 0; i < N; i += 8)
	{
		result |= ((value >> i) & 0xFF) << (N - i - 8);
	}

	return result;
}


template <typename T,
	typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	inline void writeIntoPacket(
		std::vector<unsigned char>& packet,
		size_t pos,
		T value)
{
	T* ptr = reinterpret_cast<T*>(packet.data() + pos);
	// TODO: only swap if needed
	ptr[0] = swapEndianness(value);
}

pplx::task<void> DiscordCPP::VoiceClient::play(string filename) {
	return pplx::create_task([this, filename] {
		_log.debug("creating opus encoder");
		int error;
		OpusEncoder *encoder = opus_encoder_create(SAMPLE_RATE, CHANNELS, APPLICATION, &error);
		if (error < 0) {
			throw runtime_error("failed to create opus encoder: " + string(opus_strerror(error)));
		}

		error = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(BITRATE));
		if (error < 0) {
			throw runtime_error("failed to set bitrate for opus encoder: " + string(opus_strerror(error)));
		}

		OpusDecoder *decoder = opus_decoder_create(SAMPLE_RATE, CHANNELS, &error);
		if (error < 0) {
			throw runtime_error("failed to create opus decoder: " + string(opus_strerror(error)));
		}


		_log.debug("initialising libsodium");
		if (sodium_init() == -1) {
			throw runtime_error("libsodium initialisation failed");
		}

		//unsigned char header[12];
		//unsigned char nonce[24];
		vector<unsigned char> packet(12 + MAX_PACKET_SIZE + crypto_secretbox_MACBYTES);

		ifstream file;
		file.open(filename, ios_base::binary);
//#pragma warning(suppress : 4996)
		//FILE *file = fopen(filename.c_str(), "r");
		//if (!file) {
//#pragma warning(suppress : 4996)
			//throw runtime_error("cannot open file " + filename + ": " + strerror(errno));
		//}

		ofstream fout;
		fout.open("out.wav", ios_base::binary);

		int num_opus_bytes;
		unsigned char *pcm_data = new unsigned char[FRAME_SIZE * CHANNELS * 2];
		opus_int16 *in_data;// = new opus_int16[FRAME_SIZE * CHANNELS];
		opus_int16 out[MAX_FRAME_SIZE*CHANNELS];
		//unsigned char *opus_data = new unsigned char[MAX_PACKET_SIZE];
		vector<unsigned char> opus_data(MAX_PACKET_SIZE);
		unsigned char *ciphertext;

		_log.debug("starting loop");
		
		//while (file.good()) {
		while(1) {
			//_log.debug("reading next frame");
			file.read((char *)pcm_data, FRAME_SIZE * CHANNELS * 2);
			//fread(pcm_data, sizeof(short)*CHANNELS, FRAME_SIZE, file);
			//if (feof(file))
			if(file.gcount() == 0)
				break;

			//for (unsigned int i = 0; i < CHANNELS*FRAME_SIZE; i++)
			//	in_data[i] = pcm_data[2 * i + 1] << 8 | pcm_data[2 * i];
			in_data = reinterpret_cast<opus_int16*>(pcm_data);

			//_log.debug("encoding frame");
			num_opus_bytes = opus_encode(encoder, in_data, FRAME_SIZE, opus_data.data(), MAX_PACKET_SIZE);
			if (num_opus_bytes <= 0) {
				throw runtime_error("failed to encode frame: " + string(opus_strerror(num_opus_bytes)));
			}

			int frame_size = opus_decode(decoder, opus_data.data(), num_opus_bytes, out, MAX_FRAME_SIZE, 0);
			if (frame_size <= 0) {
				throw runtime_error("failed to decode frame: " + string(opus_strerror(frame_size)));
			}

			opus_data.resize(num_opus_bytes);

			for (unsigned int i = 0; i < CHANNELS*frame_size; i++) {
				pcm_data[2 * i] = out[i] & 0xFF;
				pcm_data[2 * i + 1] = (out[i] >> 8) & 0xFF;
			}

			//fout.write((char *)opus_data.data(), opus_data.size());
			fout.write((char *)pcm_data, frame_size*CHANNELS);

			//randombytes_buf(data, 128);

			//_timestamp = (unsigned int) time(NULL);
			/*
			nonce[0] = header[0] = 0x80;	//Type
			nonce[1] = header[1] = 0x78;	//Version

			nonce[2] = header[2] = _sequence >> 8;	//Sequence
			nonce[3] = header[3] = (unsigned char) _sequence;

			nonce[4] = header[4] = _timestamp >> 24;	//Timestamp
			nonce[5] = header[5] = _timestamp >> 16;
			nonce[6] = header[6] = _timestamp >> 8;
			nonce[7] = header[7] = _timestamp;

			nonce[8] = header[8] = (unsigned char)(_ssrc >> 24);	//SSRC
			nonce[9] = header[9] = (unsigned char)(_ssrc >> 16);
			nonce[10] = header[10] = (unsigned char)(_ssrc >> 8);
			nonce[11] = header[11] = (unsigned char)_ssrc;

			for (unsigned int i = 12; i < 24; i++) {
				nonce[i] = 0;
			}
			*/

			packet[0] = 0x80;
			packet[1] = 0x78;
			writeIntoPacket(packet, 2, _sequence);
			writeIntoPacket(packet, 4, _timestamp);
			writeIntoPacket(packet, 8, _ssrc);

			_sequence++;
			_timestamp += opus_packet_get_samples_per_frame(opus_data.data(), SAMPLE_RATE);

			unsigned char nonce[crypto_secretbox_NONCEBYTES];
			memset(nonce, 0, crypto_secretbox_NONCEBYTES);
			memcpy(nonce, packet.data(), 12);

			ciphertext = new unsigned char[MAX_PACKET_SIZE + crypto_secretbox_MACBYTES];

			//crypto_secretbox_easy(opus_data, ciphertext, sizeof(ciphertext), nonce, key);
			crypto_secretbox_easy(packet.data() + 12, opus_data.data(), opus_data.size(), nonce, _secret_key.data());

			string msg;
			//msg.resize(sizeof(header) + sizeof(ciphertext), '\0');
			msg.resize(packet.size(), '\0');

			for (unsigned int i = 0; i < packet.size(); i++) {
				msg[i] = packet[i];
			}
			
			/*
			for (unsigned int i = 0; i < 12; i++) {
				msg[i] = header[i];
			}
			for (unsigned int i = 0; i < sizeof(ciphertext); i++) {
				msg[12 + i] = ciphertext[i];
			}
			*/
			//_log.debug("sending frame");
			_udp->send(msg);

			delete[] ciphertext;

			this_thread::sleep_for(chrono::milliseconds(20));
		}

		opus_encoder_destroy(encoder);

		//delete[] in_data;
		delete[] pcm_data;
		//delete[] opus_data;
		//fclose(file);
		file.close();
		fout.close();

		_log.debug("finished playing audio");
	});
}
