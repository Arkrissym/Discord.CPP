#pragma once
#include <iostream>
#include <optional>
#include <vector>

#include "DiscordObject.h"
#include "static.h"

namespace DiscordCPP {

class User;
class Discord;

class Channel : public DiscordObject {
   public:
    enum Type {
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

   private:
    /// the ChannelType of the channel
    Type type;
    /// the sorting position
    int32_t position;
    // vector<Overwrite *> permission_overwrites;
    /// the channel's name
    std::string name;
    /// the channel's icon hash
    std::string icon;

   public:
    DLL_EXPORT Channel(const json& data, const std::string& token);
    DLL_EXPORT Channel(const std::string& id, const std::string& token);
    DLL_EXPORT explicit Channel(const std::string& token);
    DLL_EXPORT Channel() = default;

    /// Delete this channel
    DLL_EXPORT void delete_channel();

    /// @return Channelname as std::string
    DLL_EXPORT explicit operator std::string() { return name; };

    /// @return the ChannelType of the channel
    DLL_EXPORT Type get_type() { return type; }
    /// @return the sorting position
    DLL_EXPORT int32_t get_position() { return position; }
    /// @return the channel's name
    DLL_EXPORT std::string get_name() { return name; }
    /// @return channel's icon hash
    DLL_EXPORT std::string get_icon() { return icon; }
};

}  // namespace DiscordCPP
