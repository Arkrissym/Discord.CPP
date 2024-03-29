#pragma once
#include <iostream>
#include <optional>
#include <vector>

#include "DiscordObject.h"

namespace DiscordCPP {
namespace ChannelType {
enum ChannelType {
    GUILD_TEXT = 0,
    DM = 1,
    GUILD_VOICE = 2,
    GROUP_DM = 3,
    GUILD_CATEGORY = 4,
    GUILD_NEWS = 5,
    GUILD_NEWS_THREAD = 10,
    GUILD_PUBLIC_THREAD = 11,
    GUILD_PRIVATE_THREAD = 12,
    GUILD_STAGE_VOICE = 13,
    GUILD_DIRECTORY = 14,
    GUILD_FORUM = 15
};
}

class User;
class Discord;

class Channel : public DiscordObject {
   public:
    /// the ChannelType of the channel
    int type;
    /// the sorting position
    int32_t position;
    // vector<Overwrite *> permission_overwrites;
    /// the channel's name
    std::string name;
    /// the channel's icon hash
    std::string icon;

    DLL_EXPORT Channel(const json& data, const std::string& token);
    DLL_EXPORT Channel(const std::string& id, const std::string& token);
    DLL_EXPORT Channel(const Channel& old);
    DLL_EXPORT Channel(){};

    DLL_EXPORT static Channel* from_json(Discord* client, const json& data, const std::string& token);

    /// Delete this channel
    DLL_EXPORT void delete_channel();

    /// @returns a copy of this channel object
    DLL_EXPORT Channel* copy();

    ///@return Channelname as std::string
    DLL_EXPORT operator std::string() { return name; };
};

}  // namespace DiscordCPP
