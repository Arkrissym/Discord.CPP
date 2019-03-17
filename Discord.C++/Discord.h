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
#include "AudioSource.h"
#include "FileAudioSource.h"
#include "Exceptions.h"
#include "MainGateway.h"
#include "DiscordObject.h"

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

	class Discord : public DiscordObject {
	protected:
		///websocket clients
		vector<MainGateway *>_gateways;
		///number of shards
		unsigned int _num_shards;
		///the user
		User *_user;
		///the guilds the user is a member
		vector<Guild *> _guilds;
		
		///array of VoiceStates
		vector<VoiceState *> _voice_states;
		
		friend VoiceClient * VoiceChannel::connect();

		DLL_EXPORT MainGateway * get_shard(unsigned int shard_id);

		DLL_EXPORT pplx::task<void> connect();
		
		DLL_EXPORT void on_websocket_incoming_message(value payload);
		DLL_EXPORT pplx::task<void> handle_raw_event(std::string event_name, value data);	//op: 0
	public:
		Logger log;

		DLL_EXPORT Discord(string token, unsigned int num_shards = 0);
		DLL_EXPORT Discord(string token, unsigned int shard_id, unsigned int num_shards);
		DLL_EXPORT virtual ~Discord();

		///called when successfully logged in
		DLL_EXPORT virtual void on_ready(User user);
		///called when a message was received
		DLL_EXPORT virtual void on_message(Message message);

		///updates the presence of user
		DLL_EXPORT pplx::task<void> update_presence(string status, Activity activity = Activity(), bool afk = false, int shard_id = -1);
	};

}
