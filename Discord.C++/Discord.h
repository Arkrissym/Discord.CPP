#pragma once
#include <cpprest/ws_client.h>
#include <cpprest/json.h>
#include <vector>

#include "Logger.h"

#include "User.h"
#include "Message.h"
#include "Channel.h"
#include "GuildChannel.h"
#include "VoiceChannel.h"
#include "DMChannel.h"
#include "Guild.h"
#include "Embed.h"
#include "Activity.h"
#include "VoiceClient.h"
#include "VoiceState.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

	using namespace web::websockets::client;
	using namespace utility;
	using namespace web::json;
	using namespace concurrency;
	using namespace std;

	class Discord {
	protected:
		///discord token
		string_t _token;
		///websocket client
		websocket_callback_client *_client;
		///heartbeat interval in milliseconds
		int _heartbeat_interval = 0;
		///current sequence number
		unsigned int _sequence_number = 0;
		///session id
		string _session_id;
		///indicator if we have a invalid session
		bool _invalid_session = false;
		///the user
		User *_user;
		///private channels of the user 
		vector<Channel *> _private_channels;
		///the guilds the user is a member
		vector<Guild *> _guilds;
		///array of servers connected to
		vector<string> _trace;
		///array of VoiceStates
		vector<VoiceState *> _voice_states;
		///the task that sends the heartbeat messages
		pplx::task<void> _heartbeat_task;
		///wether to keep the websocket alive or not
		bool _keepalive = true;

		DLL_EXPORT pplx::task<void> create_heartbeat_task();
		DLL_EXPORT pplx::task<void> connect();

		DLL_EXPORT void on_websocket_incoming_message(websocket_incoming_message msg);
		DLL_EXPORT void on_websocket_disconnnect(websocket_close_status status, string reason, error_code error);

		DLL_EXPORT pplx::task<void> handle_raw_event(std::string event_name, value data);	//op: 0
		DLL_EXPORT pplx::task<void> send_heartbeat_ack();			//op: 1
		DLL_EXPORT pplx::task<void> handle_hello_msg(value data);	//op: 10
	public:
		Logger log;

		DLL_EXPORT Discord(string token);
		DLL_EXPORT ~Discord();

		///called when successfully logged in
		DLL_EXPORT virtual void on_ready(User user);
		///called when a message was received
		DLL_EXPORT virtual void on_message(Message message);

		///updates the presence of user
		DLL_EXPORT pplx::task<void> update_presence(string status, Activity activity=Activity(), bool afk=false);
		///joins a VoiceChannel
		DLL_EXPORT VoiceClient join_voice_channel(VoiceChannel channel);
	};

}
