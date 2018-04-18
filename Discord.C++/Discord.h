#pragma once
#include <cpprest\ws_client.h>
#include <cpprest\json.h>
#include <vector>

#include "Logger.h"

#include "User.h"
#include "Message.h"
#include "Channel.h"
#include "Guild.h"

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
		User *_user;
		vector<Channel *> _private_channels;
		vector<Guild *> _guilds;
		vector<string> _trace;

		__declspec(dllexport) concurrency::task<void> create_heartbeat_task();
		__declspec(dllexport) void on_websocket_incoming_message(websocket_incoming_message msg);

		__declspec(dllexport) void handle_raw_event(std::string event_name, value data);	//op: 0
		__declspec(dllexport) void send_heartbeat_ack();			//op: 1
		__declspec(dllexport) void handle_hello_msg(value data);	//op: 10
	public:
		Logger log;

		__declspec(dllexport) Discord(string token);
		__declspec(dllexport) ~Discord();
		__declspec(dllexport) Message send_message(Channel *channel, string message, bool tts = false);
		__declspec(dllexport) virtual void on_ready(User user);
		__declspec(dllexport) virtual void on_message(Message message);
	};

}