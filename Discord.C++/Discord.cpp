#include "Discord.h"
#include "static.h"

#include <iostream>
#include <Windows.h>
#include <cpprest\http_client.h>

using namespace web::websockets::client;
using namespace web::http::client;
using namespace web::http;
using namespace utility;
using namespace web::json;
using namespace concurrency;
using namespace std;

DiscordCPP::Discord::Discord(string token) {
	_token = conversions::to_string_t(token);

	_client = websocket_callback_client();
	_client.connect(U(GATEWAY_URL));

	pplx::task<void> heartbeating = create_heartbeat_task();

	_client.set_message_handler([this](websocket_incoming_message msg) {
		on_websocket_incoming_message(msg);
	});

	log = Logger("discord");
	//log.debug("created Discord object");
}

DiscordCPP::Discord::~Discord() {
	_client.close();
	//log.debug("destroyed Discord object");
}

DiscordCPP::Message DiscordCPP::Discord::send_message(Channel channel, string message, bool tts) {
	string url = "/channels/" + channel.id + "/messages";

	http_client c(U(API_URL));
	http_request request(methods::POST);

	request.set_request_uri(uri(conversions::to_string_t(url)));
	request.headers().add(U("Authorization"), conversions::to_string_t("Bot " + conversions::to_utf8string(_token)));

	value data;
	data[U("content")] = value(conversions::to_string_t(message));
	data[U("tts")] = value(tts);

	request.set_body(data);

	Message *ret = new Message();

	c.request(request).then([this, ret](http_response response) {
		log.debug("message sent");
		
		string response_string = response.extract_utf8string().get();

		//log.debug(response_string);

		value response_data = value::parse(conversions::to_string_t(response_string));
		//log.debug(conversions::to_utf8string(response_data.at(U("content")).as_string()));
		*ret = Message(response_data, _token);
	}).wait();

	Message ret_msg = Message(*ret);
	delete ret;

	return ret_msg;
}

void DiscordCPP::Discord::on_ready(User user) {
	log.debug("on_ready");
}

void DiscordCPP::Discord::on_message(Message message) {
	log.debug("on_message");
}

task<void> DiscordCPP::Discord::create_heartbeat_task() {
	return create_task([this] {
		while (_heartbeat_interval == 0)
			Sleep(50);

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

			Sleep(_heartbeat_interval);
		}
	});
}

void DiscordCPP::Discord::on_websocket_incoming_message(websocket_incoming_message msg) {
	// handle message from server...
	task<string> str = msg.extract_string();
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

void DiscordCPP::Discord::handle_raw_event(string event_name, value data) {
	if (event_name == "READY") {
		_session_id = conversions::to_utf8string(data.at(U("session_id")).as_string());
		_user = User(data.at(U("user")));

		//_private_channels
		if (is_valid_field("private_channels")) {
			web::json::array tmp = data.at(U("private_channels")).as_array();
			for (int i = 0; i < tmp.size(); i++)
				_private_channels.push_back(Channel(tmp[i], _token));
		}

		//_guilds
		if (is_valid_field("guilds")) {
			web::json::array tmp = data.at(U("guilds")).as_array();
			for (int i = 0; i < tmp.size(); i++)
				_guilds.push_back(Guild(tmp[i], _token));
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

		create_task([this] {
			on_ready(_user);
		});
	}
	else if (event_name == "MESSAGE_CREATE") {
		create_task([this, data] {
			on_message(Message(data, _token));
		});
	}
	else if (event_name == "GUILD_CREATE") {
		Guild tmp_guild = Guild(data, _token);

		for (unsigned int i = 0; i < _guilds.size(); i++) {
			if (tmp_guild.id == _guilds[i].id) {
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
	out_json[U("d")][U("properties")][U("$os")] = value(U("Windows"));
	out_json[U("d")][U("properties")][U("$browser")] = value(U("Discord.C++"));
	out_json[U("d")][U("properties")][U("$device")] = value(U("Discord.C++"));

	//log.debug(conversions::to_utf8string(out_json.serialize()));

	websocket_outgoing_message out_msg = websocket_outgoing_message();
	out_msg.set_utf8_message(conversions::to_utf8string(out_json.serialize()));
	_client.send(out_msg).then([this] {
		log.info("Identify message has been sent");
	});
}