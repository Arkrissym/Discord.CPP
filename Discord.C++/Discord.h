#pragma once
#include <vector>

#include "Activity.h"
#include "ApplicationCommand.h"
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
#include "Interaction.h"
#include "Logger.h"
#include "MainGateway.h"
#include "Message.h"
#include "Threadpool.h"
#include "User.h"
#include "VoiceChannel.h"
#include "VoiceClient.h"
#include "VoiceState.h"
#include "static.h"

namespace DiscordCPP {

class Discord : public DiscordObject {
   protected:
    /// websocket clients
    std::vector<std::shared_ptr<MainGateway>> _gateways;
    /// number of shards
    unsigned int _num_shards;
    /// the user
    User* _user;
    /// the bot's application id
    std::string _application_id;
    /// the guilds the user is a member
    std::vector<Guild*> _guilds;

    /// array of VoiceStates
    std::vector<VoiceState*> _voice_states;

    friend std::shared_ptr<VoiceClient> VoiceChannel::connect();

    DLL_EXPORT std::shared_ptr<MainGateway> get_shard(const unsigned int shard_id);
    DLL_EXPORT Guild* get_guild(const std::string& guild_id);

    DLL_EXPORT void connect();

    DLL_EXPORT void on_websocket_incoming_message(const json& payload);
    DLL_EXPORT void handle_raw_event(const std::string& event_name, const json& data);

    /// called when successfully logged in
    virtual void on_ready(User user);
    /// called when a Message was received
    virtual void on_message(Message message);
    /// called when a Member was banned
    virtual void on_user_ban(User user, Guild guild);
    /// called when a Member was unbanned
    virtual void on_user_unban(User user, Guild guild);
    /// called when a User joins a Guild
    virtual void on_user_join(Member member, Guild guild);
    /// called when a User is removed from a Guild (leave/kick/ban)
    virtual void on_user_remove(User user, Guild guild);
    /// called when a User starts typing
    virtual void on_typing_start(User user, TextChannel channel, unsigned int timestamp);
    /// called when an interaction was created
    virtual void on_interaction(Interaction interaction);

   public:
    Logger log;

    DLL_EXPORT Discord(const std::string& token, const Intents& intents, const unsigned int num_shards = 0);
    DLL_EXPORT Discord(const std::string& token, const Intents& intents, const unsigned int shard_id, const unsigned int num_shards);
    DLL_EXPORT virtual ~Discord();

    /// Starts the bot. This will not block.
    DLL_EXPORT void start();

    /// updates the presence of user
    DLL_EXPORT void update_presence(const std::string& status, Activity activity = Activity(), const bool afk = false, const int shard_id = -1);

    /// loads all application commands
    DLL_EXPORT std::vector<ApplicationCommand> get_application_commands();
    /// loads all application commands for a guild
    DLL_EXPORT std::vector<ApplicationCommand> get_application_commands(const Guild& guild);
    /// creates a new application command
    DLL_EXPORT ApplicationCommand create_application_command(ApplicationCommand command);
};

}  // namespace DiscordCPP
