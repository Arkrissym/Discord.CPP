#include "GuildChannel.h"

#include <cpprest/http_client.h>

#include "Guild.h"
#include "static.h"

DiscordCPP::GuildChannel::GuildChannel(const json& data, const std::string& token) : DiscordCPP::TextChannel(data, token) {
    if (has_value(data, "guild_id")) {
        guild = new Guild(NULL, data.at("guild_id").get<std::string>(), token);
    }

    if (has_value(data, "parent_id")) {
        parent = new Channel(data.at("parent_id").get<std::string>(), token);
    }
}

DiscordCPP::GuildChannel::GuildChannel(const std::string& id, const std::string& token) {
    _token = token;
    std::string url = "/channels/" + id;
    *this = GuildChannel(api_call(url), token);
}

DiscordCPP::GuildChannel::GuildChannel(const GuildChannel& old) : DiscordCPP::TextChannel(old) {
    if (old.guild != NULL)
        guild = new Guild(*old.guild);
    topic = old.topic;
    if (old.parent != NULL)
        parent = new Channel(*old.parent);
}

DiscordCPP::GuildChannel::~GuildChannel() {
    if (guild != NULL)
        delete guild;
    if (parent != NULL)
        delete parent;
}
