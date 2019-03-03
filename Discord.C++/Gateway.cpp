#include "Gateway.h"
#include "static.h"
#include <time.h>
#include <chrono>
#include <exception>

using namespace web::websockets::client;
using namespace web::json;
using namespace utility::conversions;

void DiscordCPP::Gateway::start_heartbeating() {
	_heartbeat_task=pplx::create_task([this] {
		bool *keepalive = _keepalive;

		while (_heartbeat_interval == 0)
			waitFor(std::chrono::milliseconds(50)).wait();

		_last_heartbeat_ack = time(0);

		while (*keepalive) {
			if (_last_heartbeat_ack * 1000 + _heartbeat_interval * 2 < time(0) * 1000) {
				_log.warning("Gateway stopped responding. Closing and restarting websocket...");
				try {
					_client->close(websocket_close_status::going_away, U("Server not responding")).wait();
				}
				catch (std::exception &e) {
					_log.error("Cannot close websocket: " + std::string(e.what()));
				}
			}
			else {
				value payload = this->get_heartbeat_payload();
				
				try {
					this->send(payload).then([this] {
						_log.debug("Heartbeat message has been sent");
					}).wait();
				}
				catch (websocket_exception &e) {
					_log.error("Cannot send heartbeat message: " + std::string(e.what()) + " (" + std::to_string(e.error_code().value()) + ": " + e.error_code().message());
				}
				catch (const std::exception &e) {
					_log.error("Cannot send heartbeat message: " + std::string(e.what()));
				}
			}

			waitFor(std::chrono::milliseconds(_heartbeat_interval)).wait();
		}

		delete keepalive;
	});
}

void DiscordCPP::Gateway::on_websocket_disconnnect(web::websockets::client::websocket_close_status status, std::string reason, std::error_code error) {
	if (*_keepalive == false) {
		return;
	}

	_log.warning("websocket closed with status code " + std::to_string((int)status) + ": " + reason + " (" + std::to_string(error.value()) + ": " + error.message() + ")");

	pplx::create_task([this] {
		try {
			delete _client;
		}
		catch (const std::exception &e) {
			_log.error("error while deleting old websocket client: " + std::string(e.what()));
		}

		_log.info("trying to reconnect in " + std::to_string((double)_reconnect_timeout / 1000) + "s");
		waitFor(std::chrono::milliseconds(_reconnect_timeout)).wait();

		if (_reconnect_timeout == 0) {
			_reconnect_timeout = 1000;
		}
		else if (_reconnect_timeout < 90000) {	//cap timeout to about 15 min
			_reconnect_timeout = (unsigned int)(_reconnect_timeout * 1.5);
		}

		_client->connect(to_string_t(_url)).wait();
		_log.info("reconnected");
	});
}

DiscordCPP::Gateway::Gateway(std::string token) {
	_log = Logger("Discord.Gateway");

	_token = token;

	_heartbeat_interval = 0;

	_reconnect_timeout = 0;

	_keepalive = new bool;
	*_keepalive = true;

	_client = new websocket_callback_client();

	_client->set_message_handler([this](websocket_incoming_message msg) {
		pplx::task<std::string> str = msg.extract_string();
		utility::string_t message = to_string_t(str.get());

		value payload = value::parse(message);
		int op = payload.at(U("op")).as_integer();

		on_websocket_incoming_message(payload);
	});

	_client->set_close_handler([this](websocket_close_status close_status, utility::string_t reason, std::error_code error) {
		on_websocket_disconnnect(close_status, to_utf8string(reason), error);
	});
}

DiscordCPP::Gateway::~Gateway() {
	*_keepalive = false;
	delete _client;
}

void DiscordCPP::Gateway::set_message_handler(const std::function<void(web::json::value payload)>& handler) {
	_message_handler = handler;
}

pplx::task<void> DiscordCPP::Gateway::connect(std::string url) {
	_url = url;
	_log.info("connecting to websocket: " + url);
	return _client->connect(to_string_t(url)).then([this] {
		start_heartbeating();
	});
}

pplx::task<void> DiscordCPP::Gateway::send(value message) {
	web::websockets::client::websocket_outgoing_message msg;
	msg.set_utf8_message(to_utf8string(message.serialize()));
	return _client->send(msg);
}

pplx::task<void> DiscordCPP::Gateway::close() {
	*_keepalive = false;
	return _client->close();
}
