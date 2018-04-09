#pragma once
#include <cpprest\ws_client.h>
#include <cpprest\json.h>

#include "Logger.h"

#include "User.h"

namespace DiscordCPP {

	using namespace web::websockets::client;
	using namespace utility;
	using namespace web::json;
	using namespace concurrency;
	using namespace std;

	class Discord {
	protected:
		string_t _token;
		websocket_callback_client _client;
		int _heartbeat_interval = 0;
		unsigned int _sequence_number = 0;
		string _session_id;
		User _user;

		concurrency::task<void> create_heartbeat_task();
		void on_websocket_incoming_message(websocket_incoming_message msg);

		void handle_raw_event(std::string event_name, value data);	//op: 0
		void send_heartbeat_ack();			//op: 1
		void handle_hello_msg(value data);	//op: 10
	public:
		Logger log;

		Discord(std::string token);
		~Discord();
		virtual void on_ready(User user) { log.debug("on_ready"); };
	};

}