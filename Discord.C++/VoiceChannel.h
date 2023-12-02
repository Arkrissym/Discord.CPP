#pragma once

#include "Channel.h"

namespace DiscordCPP {

class Discord;
class VoiceClient;

class VoiceChannel : public Channel {
   private:
    Discord* _client = nullptr;

    /// the channel's bitrate
    int bitrate = 0;
    /// the channel's user-limit
    int user_limit = 0;
    /// the parent category channel
    std::optional<std::string> parent_id;
    std::optional<Channel> parent;

   public:
    DLL_EXPORT VoiceChannel(Discord* client, const json& data, const std::string& token);
    DLL_EXPORT VoiceChannel(Discord* client, const std::string& id, const std::string& token);
    DLL_EXPORT VoiceChannel() = default;

    /// connect to this VoiceChannel
    DLL_EXPORT std::shared_ptr<VoiceClient> connect();

    /// @return the channel's bitrate
    DLL_EXPORT int get_bitrate() { return bitrate; }
    /// @return the channel's user-limit
    DLL_EXPORT int get_user_limit() { return user_limit; }
    /// @return the parent category channel
    DLL_EXPORT std::optional<Channel> get_parent();
};

}  // namespace DiscordCPP
