#pragma once

#include <variant>

#include "Channel.h"
#include "DMChannel.h"
#include "GuildChannel.h"
#include "VoiceChannel.h"

namespace DiscordCPP {
using ChannelVariant = std::variant<DMChannel, VoiceChannel, GuildChannel, Channel>;

class ChannelHelper {
   public:
    DLL_EXPORT static ChannelVariant channel_from_json(Discord* client, const json& data, const std::string& token) {
        switch (data.at("type").get<int>()) {
            case Channel::Type::GUILD_TEXT:
            case Channel::Type::GUILD_NEWS:
                return GuildChannel(data, token);
            case Channel::Type::GUILD_VOICE:
                return VoiceChannel(client, data, token);
            case Channel::Type::DM:
            case Channel::Type::GROUP_DM:
                return DMChannel(data, token);
            default:
                return Channel(data, token);
        }
    }

    DLL_EXPORT static std::string get_channel_id(const ChannelVariant& channel) {
        return std::visit([](auto& c) { return c.get_id(); }, channel);
    }
};
}  // namespace DiscordCPP