#pragma once
#include "TextChannel.h"
#include "static.h"

namespace DiscordCPP {

class Guild;

class GuildChannel : public TextChannel {
   public:
    ///the guild the channel belongs to
    Guild* guild = NULL;
    ///the parent category channel
    Channel* parent;

    DLL_EXPORT GuildChannel(const json& data, const std::string& token);
    DLL_EXPORT GuildChannel(const std::string& id, const std::string& token);
    DLL_EXPORT GuildChannel(const GuildChannel& old);
    DLL_EXPORT GuildChannel(){};
    DLL_EXPORT ~GuildChannel();
};

}  // namespace DiscordCPP
