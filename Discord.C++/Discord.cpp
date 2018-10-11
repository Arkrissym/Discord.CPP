#include "Discord.h"
#include "static.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <cpprest/http_client.h>

using namespace web::websockets::client;
using namespace web::http::client;
using namespace web::http;
using namespace utility;
using namespace web::json;
using namespace concurrency;
using namespace std;

DiscordCPP::Discord::Discord(string token) {
	_token = conversions::to_string_t(token);

	log = Logger("discord");

	pplx::task<void> heartbeating = create_heartbeat_task();

	connect();

	srand((unsigned int)time(NULL));

	//log.debug("created Discord object");
}

DiscordCPP::Discord::~Discord() {
	_client->close(websocket_close_status::normal, U("user input")).then([this] {
		delete _client;
	});
	//log.debug("destroyed Discord object");

	delete _user;
	for (unsigned int i = 0; i < _private_channels.size(); i++) {
		delete _private_channels[i];
	}
	for (unsigned int i = 0; i < _guilds.size(); i++) {
		delete _guilds[i];
	}
}

///	@param[in]	user	the User
void DiscordCPP::Discord::on_ready(User user) {
	log.debug("on_ready");
}

///	@param[in]	message	the Message that was received
void DiscordCPP::Discord::on_message(Message message) {
	log.debug("on_message");
}

/**	@param[in]	status		the new status (see DiscordStatus)
	@param[in]	activity	(optional) the Activity
	@param[in]	afk			(optional) wether the bot/user is afk or not
*/
pplx::task<void> DiscordCPP::Discord::update_presence(string status, Activity activity, bool afk) {
	value presence;

	presence[U("op")] = value(3);

	presence[U("d")][U("since")] = value();
	if (activity.type == ActivityTypes::NoActivity)
		presence[U("d")][U("game")] = value();
	else
		presence[U("d")][U("game")] = activity.to_json();
	
	presence[U("d")][U("status")] = value(conversions::to_string_t(status));
	presence[U("d")][U("afk")] = value(afk);

	websocket_outgoing_message msg;
	msg.set_utf8_message(conversions::to_utf8string(presence.serialize()));

	return _client->send(msg);
}

pplx::task<void> DiscordCPP::Discord::create_heartbeat_task() {
	return pplx::create_task([this] {
		while (_heartbeat_interval == 0)
			//this_thread::sleep_for(chrono::milliseconds(50));
			waitFor(chrono::milliseconds(50)).wait();

		_last_heartbeat_ack = time(0);

		while (1) {
			if (_last_heartbeat_ack * 1000 + _heartbeat_interval * 2 < time(0) * 1000) {
				log.warning("Gateway stopped responding. Closing and restarting websocket...");
				try {
					_client->close(websocket_close_status::going_away, U("Server not responding")).wait();
				}
				catch (exception &e) {
					log.error("Cannot close websocket: " + string(e.what()));
				}
			}
			else {
				websocket_outgoing_message heartbeat_msg = websocket_outgoing_message();
				if (_sequence_number == 0)
					heartbeat_msg.set_utf8_message("{\"op\": 1, \"d\": null}");
				else {
					string str = "{\"op\": 1, \"d\": " + to_string(_sequence_number) + "}";
					//log.debug(str);
					heartbeat_msg.set_utf8_message(str);
				}

				try {
					_client->send(heartbeat_msg).then([this] {
						log.debug("Heartbeat message has been sent");
					}).wait();
				}
				catch (const std::exception &e) {
					log.error("Cannot send heartbeat message: " + string(e.what()));
				}
			}

			//this_thread::sleep_for(chrono::milliseconds(_heartbeat_interval));
			waitFor(chrono::milliseconds(_heartbeat_interval)).wait();
		}
	});
}

pplx::task<void> DiscordCPP::Discord::connect() {
	return pplx::create_task([this] {
		log.info("connecting to websocket: " + string(GATEWAY_URL));

		_client = new websocket_callback_client();
		_client->connect(U(GATEWAY_URL));

		_client->set_message_handler([this](websocket_incoming_message msg) {
			on_websocket_incoming_message(msg);
		});

		_client->set_close_handler([this](websocket_close_status close_status, string_t reason, error_code error) {
			on_websocket_disconnnect(close_status, conversions::to_utf8string(reason), error);
		});
	});
}

void DiscordCPP::Discord::on_websocket_incoming_message(websocket_incoming_message msg) {
	// handle message from server...
	pplx::task<string> str = msg.extract_string();
	string_t message = conversions::to_string_t(str.get());

	value obj = value::parse(message);
	int op = obj.at(U("op")).as_integer();

	if ( (obj.has_field(U("s"))) && (!obj.at(U("s")).is_null()) ) {
		_sequence_number = obj.at(U("s")).as_integer();
	}

	switch (op) {
		case 0:
			handle_raw_event(conversions::to_utf8string(obj.at(U("t")).as_string()), obj.at(U("d")));
			break;
		case 1:
			send_heartbeat_ack();
			break;
		case 9:
			_invalid_session = true;
			break;
		case 10:
			handle_hello_msg(obj.at(U("d")));
			break;
		case 11:
			log.debug("received heartbeat ACK");
			_last_heartbeat_ack = time(0);
			break;
		default:
			//print message
			log.debug(str.get());

			log.warning("ignoring payload with op-code: " + to_string(op));
			break;
	}
}

void DiscordCPP::Discord::on_websocket_disconnnect(websocket_close_status status, string reason, error_code error) {
	log.warning("websocket closed with status code " + to_string((int)status) + ": " + reason + " (" + to_string(error.value()) + ": " + error.message() + ")");

	pplx::create_task([this] {
		try {
			delete _client;
		}
		catch (const std::exception &e) {
			log.error("error while deleting old websocket client: " + string(e.what()));
		}
	}).then([this] {
		log.info("trying to reconnect in " + to_string((double)_reconnect_timeout / 1000) + "s");
	}).then([this] {
		//this_thread::sleep_for(chrono::milliseconds(_reconnect_timeout));
		waitFor(chrono::milliseconds(_reconnect_timeout)).wait();
		
		if (_reconnect_timeout == 0) {
			_reconnect_timeout = 1000;
		}
		else {
			_reconnect_timeout = (unsigned int)(_reconnect_timeout * 1.5);
		}
		connect().wait();
		log.info("reconnected");
	});
}

pplx::task<void> DiscordCPP::Discord::handle_raw_event(string event_name, value data) {
	return pplx::create_task([this, event_name, data] {
		if (event_name == "READY") {
			_reconnect_timeout = 0;
			_last_heartbeat_ack = time(0);

			_session_id = conversions::to_utf8string(data.at(U("session_id")).as_string());
			_user = new User(data.at(U("user")), _token);

			//_private_channels
			if (is_valid_field("private_channels")) {
				web::json::array tmp = data.at(U("private_channels")).as_array();
				for (unsigned int i = 0; i < tmp.size(); i++)
					_private_channels.push_back(new DMChannel(tmp[i], _token));
			}

			//_guilds
			if (is_valid_field("guilds")) {
				web::json::array tmp = data.at(U("guilds")).as_array();
				for (unsigned int i = 0; i < tmp.size(); i++)
					_guilds.push_back(new Guild(tmp[i], _token));
			}

			web::json::array tmp = data.at(U("_trace")).as_array();
			string str = "[ ";
			for (unsigned int i = 0; i < tmp.size(); i++) {
				_trace.push_back(conversions::to_utf8string(tmp[i].as_string()));
				if (i == 0)
					str = str + conversions::to_utf8string(tmp[i].as_string());
				else
					str = str + ", " + conversions::to_utf8string(tmp[i].as_string());
			}

			log.info("connected to: " + str + " ]");
			log.info("session id: " + _session_id);

			pplx::create_task([this] {
				try {
					on_ready(User(*_user));
				}
				catch (const std::exception &e) {
					log.error("ignoring exception in on_ready: " + string(e.what()));
				}
			});
		}
		else if (event_name == "RESUMED") {
			_reconnect_timeout = 0;
			_last_heartbeat_ack = time(0);

			web::json::array tmp = data.at(U("_trace")).as_array();
			string str = "[ ";
			for (unsigned int i = 0; i < tmp.size(); i++) {
				_trace.push_back(conversions::to_utf8string(tmp[i].as_string()));
				if (i == 0)
					str = str + conversions::to_utf8string(tmp[i].as_string());
				else
					str = str + ", " + conversions::to_utf8string(tmp[i].as_string());
			}

			log.info("successfully resumed session " + _session_id + " with trace " + str + " ]");
		}
		else if (event_name == "MESSAGE_CREATE") {
			pplx::create_task([this, data] {
				try {
					on_message(Message(data, _token));
				}
				catch (const std::exception &e) {
					log.error("ignoring exception in on_message: " + string(e.what()));
				}
			});
		}
		else if (event_name == "GUILD_CREATE") {
			Guild *tmp_guild = new Guild(data, _token);

			for (unsigned int i = 0; i < _guilds.size(); i++) {
				if (tmp_guild->id == _guilds[i]->id) {
					_guilds[i] = tmp_guild;
					log.debug("Updated guild data");
					return;
				}
			}

			_guilds.push_back(tmp_guild);
			log.debug("data of new guild added");
		}
		else {
			log.warning("ignoring event: " + event_name);
		}
	});
}

pplx::task<void> DiscordCPP::Discord::send_heartbeat_ack() {
	return pplx::create_task([this] {
		value out_json;
		out_json[U("op")] = value(11);
		websocket_outgoing_message out_msg = websocket_outgoing_message();
		out_msg.set_utf8_message(conversions::to_utf8string(out_json.serialize()));
		_client->send(out_msg).then([this] {
			log.debug("Heartbeat ACK message has been sent");
		});
	});
}

pplx::task<void> DiscordCPP::Discord::handle_hello_msg(value data) {
	return pplx::create_task([this, data] {
		_heartbeat_interval = data.at(U("heartbeat_interval")).as_integer();
		log.debug("set heartbeat_interval: " + to_string(_heartbeat_interval));

		value out_json;
		websocket_outgoing_message out_msg = websocket_outgoing_message();

		if (_sequence_number > 0) {
			unsigned int seq = _sequence_number;

			log.info("trying to resume session");

			out_json[U("op")] = value(6);
			out_json[U("d")][U("token")] = value(_token);
			out_json[U("d")][U("session_id")] = value(conversions::to_string_t(_session_id));
			out_json[U("d")][U("seq")] = value(seq);

			out_msg.set_utf8_message(conversions::to_utf8string(out_json.serialize()));
			_client->send(out_msg).then([this] {
				log.info("Resume payload has been sent");
			}).wait();

			while (_invalid_session == false) {
				if (_sequence_number > seq) {
					return;
				}
				//this_thread::sleep_for(chrono::milliseconds(100));
				waitFor(chrono::milliseconds(100)).wait();
			}

			log.info("cannot resume session");

			//this_thread::sleep_for(chrono::milliseconds(1000 + rand() % 4001));
			waitFor(chrono::milliseconds(1000 + rand() % 4001)).wait();
		}

		out_json = value();
		out_json[U("op")] = value(2);

		out_json[U("d")][U("token")] = value(_token);
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

		//log.debug(conversions::to_utf8string(out_json.serialize()));

		out_msg = websocket_outgoing_message();
		out_msg.set_utf8_message(conversions::to_utf8string(out_json.serialize()));
		_client->send(out_msg).then([this] {
			log.info("Identify payload has been sent");
		});
	});
}
