#pragma once
#include "TextChannel.h"
#include "static.h"

namespace DiscordCPP {

class Guild;

class GuildChannel : public TextChannel {
   private:
    /// the guild the channel belongs to
    std::optional<std::string> guild_id;
    Guild* guild = nullptr;
    /// the parent category channel
    std::optional<std::string> parent_id;
    std::optional<Channel> parent;

   public:
    DLL_EXPORT GuildChannel(const json& data, const std::string& token);
    DLL_EXPORT GuildChannel(const std::string& id, const std::string& token);
    DLL_EXPORT GuildChannel() = default;

    /// @return the guild the channel belongs to
    DLL_EXPORT std::optional<Guild> get_guild();
    /// @return the parent category channel
    DLL_EXPORT std::optional<Channel> get_parent();
};

}  // namespace DiscordCPP
