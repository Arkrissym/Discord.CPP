#include "GuildChannel.h"

#include "Guild.h"
#include "static.h"

DiscordCPP::GuildChannel::GuildChannel(const json& data, const std::string& token)
    : DiscordCPP::TextChannel(data, token) {
    guild_id = get_optional<std::string>(data, "guild_id");
    parent_id = get_optional<std::string>(data, "parent_id");
}

DiscordCPP::GuildChannel::GuildChannel(const std::string& id, const std::string& token) {
    std::string url = "/channels/" + id;
    *this = GuildChannel(api_call(url), token);
}

std::optional<DiscordCPP::Guild> DiscordCPP::GuildChannel::get_guild() {
    if (guild == nullptr && guild_id.has_value()) {
        guild = new Guild(nullptr, guild_id.value(), get_token());
    }
    return (guild != nullptr) ? *guild : std::make_optional<DiscordCPP::Guild>();
}

std::optional<DiscordCPP::Channel> DiscordCPP::GuildChannel::get_parent() {
    if (!parent.has_value() && parent_id.has_value()) {
        parent.emplace(parent_id.value(), get_token());
    }
    return parent;
}
