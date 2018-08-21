#include "Discord.h"
#include "static.h"

#include <iostream>
#include <chrono>
#include <thread>
//#include <Windows.h>
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

	_client = websocket_callback_client();
	_client.connect(U(GATEWAY_URL));

	pplx::task<void> heartbeating = create_heartbeat_task();

	_client.set_message_handler([this](websocket_incoming_message msg) {
		on_websocket_incoming_message(msg);
	});

	_client.set_close_handler([this](websocket_close_status close_status, string_t reason, error_code error) {
		on_websocket_disconnnect(close_status, conversions::to_utf8string(reason), error);
	});

	//log.debug("created Discord object");
}

DiscordCPP::Discord::~Discord() {
	_client.close(websocket_close_status::normal, U("user input"));
	//log.debug("destroyed Discord object");

	delete _user;
	for (int i = 0; i < _private_channels.size(); i++) {
		delete _private_channels[i];
	}
	for (int i = 0; i < _guilds.size(); i++) {
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

	return _client.send(msg);
}

pplx::task<void> DiscordCPP::Discord::create_heartbeat_task() {
	return pplx::create_task([this] {
		while (_heartbeat_interval == 0)
			this_thread::sleep_for(chrono::milliseconds(50));

		while (1) {
			websocket_outgoing_message heartbeat_msg = websocket_outgoing_message();
			if(_sequence_number == 0)
				heartbeat_msg.set_utf8_message("{\"op\": 1, \"d\": null}");
			else {
				string str = "{\"op\": 1, \"d\": " + to_string(_sequence_number) + "}";
				//log.debug(str);
				heartbeat_msg.set_utf8_message(str);
			}

			_client.send(heartbeat_msg).then([this] {
				log.debug("Heartbeat message has been sent");
			});

			this_thread::sleep_for(chrono::milliseconds(_heartbeat_interval));
		}
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
		case 10:
			handle_hello_msg(obj.at(U("d")));
			break;
		case 11:
			log.debug("received heartbeat ACK");
			break;
		default:
			//print message
			log.debug(str.get());

			log.warning("ignoring payload with op-code: " + to_string(op));
			break;
	}
}

void DiscordCPP::Discord::on_websocket_disconnnect(websocket_close_status status, string reason, error_code error) {
	log.warning("websocket closed with status code " + to_string((int)status) + ": " + reason);

	_client.connect(U(GATEWAY_URL));
}

void DiscordCPP::Discord::handle_raw_event(string event_name, value data) {
	if (event_name == "READY") {
		_session_id = conversions::to_utf8string(data.at(U("session_id")).as_string());
		_user = new User(data.at(U("user")), _token);

		//_private_channels
		if (is_valid_field("private_channels")) {
			web::json::array tmp = data.at(U("private_channels")).as_array();
			for (int i = 0; i < tmp.size(); i++)
				_private_channels.push_back(new DMChannel(tmp[i], _token));
		}

		//_guilds
		if (is_valid_field("guilds")) {
			web::json::array tmp = data.at(U("guilds")).as_array();
			for (int i = 0; i < tmp.size(); i++)
				_guilds.push_back(new Guild(tmp[i], _token));
		}

		web::json::array tmp = data.at(U("_trace")).as_array();
		string str = "[ ";
		for (int i = 0; i < tmp.size(); i++) {
			_trace.push_back(conversions::to_utf8string(tmp[i].as_string()));
			if (i == 0)
				str = str + conversions::to_utf8string(tmp[i].as_string());
			else
				str = str + ", " + conversions::to_utf8string(tmp[i].as_string());
		}
		
		log.info("connected to: " + str + " ]");

		pplx::create_task([this] {
			try {
				on_ready(User(*_user));
			}
			catch (const std::exception &e) {
				log.error("ignoring exception in on_ready: " + string(e.what()));
			}
		});
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
}

void DiscordCPP::Discord::send_heartbeat_ack() {
	value out_json;
	out_json[U("op")] = value(11);
	websocket_outgoing_message out_msg = websocket_outgoing_message();
	out_msg.set_utf8_message(conversions::to_utf8string(out_json.serialize()));
	_client.send(out_msg).then([this] {
		log.debug("Heartbeat ACK message has been sent");
	});
}

void DiscordCPP::Discord::handle_hello_msg(value data) {
	_heartbeat_interval = data.at(U("heartbeat_interval")).as_integer();
	log.debug("set heartbeat_interval: " + to_string(_heartbeat_interval));

	value out_json;
	out_json[U("op")] = value(2);

	out_json[U("d")][U("token")] = value(_token);
#ifdef _WIN64
	out_json[U("d")][U("properties")][U("$os")] = value(U("Windows(64 bit)"));
#elif _WIN32
	out_json[U("d")][U("properties")][U("$os")] = value(U("Windows(32 bit)"));
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

	websocket_outgoing_message out_msg = websocket_outgoing_message();
	out_msg.set_utf8_message(conversions::to_utf8string(out_json.serialize()));
	_client.send(out_msg).then([this] {
		log.info("Identify message has been sent");
	});
}
