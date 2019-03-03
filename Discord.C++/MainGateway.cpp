#include "MainGateway.h"
#include "static.h"
//#include <vector>

using namespace web::json;
using namespace utility::conversions;

value DiscordCPP::MainGateway::get_heartbeat_payload() {
	value payload = value();

	payload[U("op")] = value(1);

	if (_sequence_number == 0)
		payload[U("d")] = value::null();
	else
		payload[U("d")] = value(_sequence_number);

	return payload;
}

void DiscordCPP::MainGateway::on_websocket_incoming_message(web::json::value payload) {
	int op = payload.at(U("op")).as_integer();

	if ((payload.has_field(U("s"))) && (!payload.at(U("s")).is_null())) {
		_sequence_number = payload.at(U("s")).as_integer();
	}

	switch (op) {
		case 0:
			if (to_utf8string(payload.at(U("t")).as_string()) == "READY") {
				_reconnect_timeout = 0;
				_last_heartbeat_ack = time(0);

				_invalid_session = false;

				_session_id = to_utf8string(payload.at(U("d")).at(U("session_id")).as_string());

				web::json::array tmp = payload.at(U("d")).at(U("_trace")).as_array();
				std::string str = "[ ";
				for (unsigned int i = 0; i < tmp.size(); i++) {
					_trace.push_back(to_utf8string(tmp[i].as_string()));
					if (i == 0)
						str = str + to_utf8string(tmp[i].as_string());
					else
						str = str + ", " + to_utf8string(tmp[i].as_string());
				}

				_log.info("connected to: " + str + " ]");
				_log.info("session id: " + _session_id);
			}
			else if (to_utf8string(payload.at(U("t")).as_string()) == "RESUMED") {
				_reconnect_timeout = 0;
				_last_heartbeat_ack = time(0);

				web::json::array tmp = payload.at(U("d")).at(U("_trace")).as_array();
				std::string str = "[ ";
				for (unsigned int i = 0; i < tmp.size(); i++) {
					_trace.push_back(to_utf8string(tmp[i].as_string()));
					if (i == 0)
						str = str + to_utf8string(tmp[i].as_string());
					else
						str = str + ", " + to_utf8string(tmp[i].as_string());
				}

				_log.info("successfully resumed session " + _session_id + " with trace " + str + " ]");
			}
			break;
		case 1:
			send_heartbeat_ack();
			break;
		case 7:
			_log.info("received opcode 7: reconnecting to the gateway");
			_client->close(web::websockets::client::websocket_close_status::normal, U("received opcode 7"));
			break;
		case 9:
			_invalid_session = true;
			break;
		case 10:
			_heartbeat_interval = payload.at(U("d")).at(U("heartbeat_interval")).as_integer();
			_log.debug("set heartbeat_interval: " + std::to_string(_heartbeat_interval));
			identify();
			break;
		case 11:
			_log.debug("received heartbeat ACK");
			_last_heartbeat_ack = time(0);
			break;
		default:
			break;
	}

	_message_handler(payload);
}

pplx::task<void> DiscordCPP::MainGateway::send_heartbeat_ack() {
	return pplx::create_task([this] {
		value ack;
		ack[U("op")] = value(11);
		this->send(ack).then([this] {
			_log.debug("Heartbeat ACK message has been sent");
		});
	});
}

pplx::task<void> DiscordCPP::MainGateway::identify() {
	return pplx::create_task([this] {
		value out_json;

		if (_sequence_number > 0) {
			unsigned int seq = _sequence_number;

			_log.info("trying to resume session");

			out_json[U("op")] = value(6);
			out_json[U("d")][U("token")] = value(to_string_t(_token));
			out_json[U("d")][U("session_id")] = value(to_string_t(_session_id));
			out_json[U("d")][U("seq")] = value(seq);

			this->send(out_json).then([this] {
				_log.info("Resume payload has been sent");
			}).wait();

			while (_invalid_session == false) {
				if (_sequence_number > seq) {
					return;
				}
				//this_thread::sleep_for(chrono::milliseconds(100));
				waitFor(std::chrono::milliseconds(100)).wait();
			}

			_log.info("cannot resume session");

			//this_thread::sleep_for(chrono::milliseconds(1000 + rand() % 4001));
			waitFor(std::chrono::milliseconds(1000 + rand() % 4001)).wait();
		}

		out_json = value();
		out_json[U("op")] = value(2);

		out_json[U("d")][U("token")] = value(to_string_t(_token));
		out_json[U("d")][U("shard")][0] = value(_shard_id);
		out_json[U("d")][U("shard")][1] = value(_num_shards);
#ifdef _WIN32
		out_json[U("d")][U("properties")][U("$os")] = value(U("Windows"));
#elif __APPLE__
		out_json[U("d")][U("properties")][U("$os")] = value(U("OS X"));
#elif __linux__
		out_json[U("d")][U("properties")][U("$os")] = value(U("Linux"));
#elif __unix__
		out_json[U("d")][U("properties")][U("$os")] = value(U("Unix"));
#elif __posix
		out_json[U("d")][U("properties")][U("$os")] = value(U("POSIX"));
#endif
		out_json[U("d")][U("properties")][U("$browser")] = value(U("Discord.C++"));
		out_json[U("d")][U("properties")][U("$device")] = value(U("Discord.C++"));

		this->send(out_json).then([this] {
			_log.info("Identify payload has been sent");
		});
	});
}

DiscordCPP::MainGateway::MainGateway(std::string token, int shard_id, unsigned int num_shards) : Gateway::Gateway(token) {
	_log = Logger("Discord.MainGateway (shard id: [" + std::to_string(shard_id) + ", " + std::to_string(num_shards) + "] )");

	_invalid_session = false;

	_shard_id = shard_id;
	_num_shards = num_shards;

	_sequence_number = 0;

	_message_handler = [this](value payload) {
		_log.info("dummy message handler called");
	};
}

DiscordCPP::MainGateway::~MainGateway() {
}
