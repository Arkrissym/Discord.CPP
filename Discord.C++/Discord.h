#pragma once
#include <cpprest/json.h>
#include <cpprest/ws_client.h>

#include <future>
#include <vector>

#include "Activity.h"
#include "AudioSource.h"
#include "Channel.h"
#include "DMChannel.h"
#include "DiscordObject.h"
#include "Embed.h"
#include "Exceptions.h"
#include "FFmpegAudioSource.h"
#include "FileAudioSource.h"
#include "Guild.h"
#include "GuildChannel.h"
#include "Intents.h"
#include "Logger.h"
#include "MainGateway.h"
#include "Message.h"
#include "Threadpool.h"
#include "User.h"
#include "VoiceChannel.h"
#include "VoiceClient.h"
#include "VoiceState.h"
#include "static.h"

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

class Discord : public DiscordObject {
   protected:
    ///websocket clients
    vector<MainGateway*> _gateways;
    ///number of shards
    unsigned int _num_shards;
    ///the user
    User* _user;
    ///the guilds the user is a member
    vector<Guild*> _guilds;

    ///array of VoiceStates
    vector<VoiceState*> _voice_states;

    friend VoiceClient* VoiceChannel::connect();

    DLL_EXPORT MainGateway* get_shard(const unsigned int shard_id);
    DLL_EXPORT Guild* get_guild(const string& guild_id);

    DLL_EXPORT void connect();

    DLL_EXPORT void on_websocket_incoming_message(const value& payload);
    DLL_EXPORT void handle_raw_event(const std::string& event_name, const value& data);

   public:
    Logger log;

    DLL_EXPORT Discord(const string& token, const Intents& intents, const unsigned int num_shards = 0);
    DLL_EXPORT Discord(const string& token, const Intents& intents, const unsigned int shard_id, const unsigned int num_shards);
    DLL_EXPORT virtual ~Discord();

    ///called when successfully logged in
    DLL_EXPORT virtual void on_ready(User user);
    ///called when a Message was received
    DLL_EXPORT virtual void on_message(Message message);
    ///called when a Member was banned
    DLL_EXPORT virtual void on_user_ban(User user, Guild guild);
    ///called when a Member was unbanned
    DLL_EXPORT virtual void on_user_unban(User user, Guild guild);
    ///called when a User joins a Guild
    DLL_EXPORT virtual void on_user_join(Member member, Guild guild);
    ///called when a User is removed from a Guild (leave/kick/ban)
    DLL_EXPORT virtual void on_user_remove(User user, Guild guild);
    ///called when a User starts typing
    DLL_EXPORT virtual void on_typing_start(User user, TextChannel channel, unsigned int timestamp);

    ///updates the presence of user
    DLL_EXPORT void update_presence(const string& status, Activity activity = Activity(), const bool afk = false, const int shard_id = -1);
};

}  // namespace DiscordCPP
