#pragma once

#include "Channel.h"

namespace DiscordCPP {

class Guild;
class Discord;
class VoiceClient;

class VoiceChannel : public Channel {
   private:
    Discord* _client;

   public:
    ///the channel's bitrate
    int bitrate;
    ///the channel's user-limit
    int user_limit;
    ///the parent category channel
    Channel* parent = NULL;
    ///the guild the channel belongs to
    Guild* guild = NULL;

    DLL_EXPORT VoiceChannel(Discord* client, const json& data, const std::string& token);
    DLL_EXPORT VoiceChannel(Discord* client, const std::string& id, const std::string& token);
    DLL_EXPORT VoiceChannel(const VoiceChannel& old);
    DLL_EXPORT VoiceChannel(){};
    DLL_EXPORT ~VoiceChannel();

    ///connect to this VoiceChannel
    DLL_EXPORT VoiceClient* connect();
};

}  // namespace DiscordCPP
