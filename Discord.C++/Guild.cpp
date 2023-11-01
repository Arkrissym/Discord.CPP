#include "Guild.h"

#include <string>

#include "DMChannel.h"
#include "TextChannel.h"
#include "VoiceChannel.h"
#include "static.h"

DiscordCPP::Guild::Guild(Discord* client, const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token, data.at("id").get<std::string>()),
      client(client) {
    name = get_or_else<std::string>(data, "name", "");
    icon = get_or_else<std::string>(data, "icon", "");
    splash = get_or_else<std::string>(data, "splash", "");
    permissions = get_or_else<int>(data, "permissions", 0);
    region = get_or_else<std::string>(data, "region", "");
    afk_timeout = get_or_else<int>(data, "afk_timeout", 0);
    embed_enabled = get_or_else<bool>(data, "embed_enabled", false);
    verification_level = get_or_else<int>(data, "verification_level", 0);
    default_message_notifications = get_or_else<int>(data, "default_message_notifications", 0);
    explicit_content_filter = get_or_else<int>(data, "explicit_content_filter", 0);
    mfa_level = get_or_else<int>(data, "mfa_level", 0);
    application_id = get_or_else<std::string>(data, "application_id", "");
    widget_enabled = get_or_else<bool>(data, "widget_enabled", false);
    joined_at = get_or_else<std::string>(data, "joined_at", "");
    large = get_or_else<bool>(data, "large", false);
    unavailable = get_or_else<bool>(data, "unavailable", false);
    member_count = get_or_else<int>(data, "member_count", 0);

    owner_id = get_optional<std::string>(data, "owner_id");
    afk_channel_id = get_optional<std::string>(data, "afk_channel_id");
    embed_channel_id = get_optional<std::string>(data, "embed_channel_id");

    // roles

    // emojis

    if (has_value(data, "features")) {
        for (const json& feature : data.at("features")) {
            features.push_back(feature.get<std::string>());
        }
    }

    widget_channel_id = get_optional<std::string>(data, "widget_channel_id");
    system_channel_id = get_optional<std::string>(data, "system_channel_id");

    // voice_states

    if (has_value(data, "members")) {
        for (json member : data.at("members")) {
            members.push_back(Member(member, token));
        }
    }

    if (has_value(data, "channels")) {
        for (json channel : data.at("channels")) {
            channels.push_back(Channel::from_json(client, channel, token));
        }
    }

    // presences
}

DiscordCPP::Guild::Guild(Discord* client, const std::string& id, const std::string& token)
    : DiscordCPP::DiscordObject(token) {
    std::string url = "/guilds/" + id;

    *this = Guild(client, api_call(url), token);
}

DiscordCPP::Guild::Guild(const Guild& old)
    : DiscordCPP::DiscordObject(old) {
    name = old.name;
    icon = old.icon;
    splash = old.splash;
    owner = old.owner;
    permissions = old.permissions;
    region = old.region;
    afk_channel = old.afk_channel;
    afk_timeout = old.afk_timeout;
    embed_enabled = old.embed_enabled;
    embed_channel = old.embed_channel;
    verification_level = old.verification_level;
    default_message_notifications = old.default_message_notifications;
    explicit_content_filter = old.explicit_content_filter;
    // roles
    // emojis
    features = old.features;
    mfa_level = old.mfa_level;
    application_id = old.application_id;
    widget_enabled = old.widget_enabled;
    widget_channel = old.widget_channel;
    system_channel = old.system_channel;
    joined_at = old.joined_at;
    large = old.large;
    unavailable = old.unavailable;
    member_count = old.member_count;
    // voice_states
    members = old.members;
    for (auto channel : old.channels) {
        channels.push_back(channel->copy());
    }
    // presences
}

DiscordCPP::Guild::~Guild() {
    for (auto& channel : channels) {
        delete channel;
    }
}

void DiscordCPP::Guild::_add_channel(Channel* channel) {
    channels.push_back(channel);
}

void DiscordCPP::Guild::_update_channel(Channel* channel) {
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->get_id() == channel->get_id()) {
            delete channels[i];
            channels.erase(channels.begin() + i);
            channels.push_back(channel);
            break;
        }
    }
}

void DiscordCPP::Guild::_remove_channel(const std::string& channel_id) {
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->get_id() == channel_id) {
            delete channels[i];
            channels.erase(channels.begin() + i);
            break;
        }
    }
}

void DiscordCPP::Guild::_add_member(Member member) {
    members.push_back(member);
    member_count += 1;
}

void DiscordCPP::Guild::_update_member(Member member) {
    for (size_t i = 0; i < members.size(); i++) {
        if (members[i].get_id() == member.get_id()) {
            members.erase(members.begin() + i);
            members.push_back(member);
        }
    }
}

void DiscordCPP::Guild::_remove_member(const std::string& member_id) {
    for (size_t i = 0; i < members.size(); i++) {
        if (members[i].get_id() == member_id) {
            members.erase(members.begin() + i);
            member_count -= 1;
        }
    }
}

///@throws HTTPError
void DiscordCPP::Guild::leave() {
    std::string url = "/guilds/@me/guilds/" + get_id();
    api_call(url, "DEL");
}

///@throws HTTPError
void DiscordCPP::Guild::delete_guild() {
    std::string url = "/guilds/" + get_id();
    api_call(url, "DEL");
}

/**	@param[in]	user	User to kick
    @throws HTTPError
*/
void DiscordCPP::Guild::kick(const User& user) {
    std::string url = "/guilds/" + get_id() + "/members/" + user.get_id();
    api_call(url, "DEL");
}

/**	@param[in]	user				User to ban
    @param[in]	reason				reason, why the User should be banned
    @param[in]	delete_message_days	(optional) number of days to delete messages from user (0-7)
    @throws HTTPError
*/
void DiscordCPP::Guild::ban(const User& user, const std::string& reason, const int delete_message_days) {
    std::string url = "/guilds/" + get_id() + "/bans/" + user.get_id() + "?delete-message-days=" + std::to_string(delete_message_days) + "&reason=" + urlencode(reason);
    api_call(url, "PUT");
}

/**	@param[in]	user	User to kick
    @throws HTTPError
*/
void DiscordCPP::Guild::unban(const User& user) {
    std::string url = "/guilds/" + get_id() + "/bans/" + user.get_id();
    api_call(url, "DEL");
}

std::optional<DiscordCPP::User> DiscordCPP::Guild::get_owner() {
    if (!owner.has_value() && owner_id.has_value()) {
        owner.emplace(owner_id.value(), get_token());
    }
    return owner;
}

std::optional<DiscordCPP::VoiceChannel> DiscordCPP::Guild::get_afk_channel() {
    if (!afk_channel.has_value() && afk_channel_id.has_value()) {
        afk_channel.emplace(client, afk_channel_id.value(), get_token());
    }
    return afk_channel;
}

std::optional<DiscordCPP::GuildChannel> DiscordCPP::Guild::get_embed_channel() {
    if (!embed_channel.has_value() && embed_channel_id.has_value()) {
        embed_channel.emplace(embed_channel_id.value(), get_token());
    }
    return embed_channel;
}

std::optional<DiscordCPP::GuildChannel> DiscordCPP::Guild::get_widget_channel() {
    if (!widget_channel.has_value() && widget_channel_id.has_value()) {
        widget_channel.emplace(widget_channel_id.value(), get_token());
    }
    return widget_channel;
}

std::optional<DiscordCPP::GuildChannel> DiscordCPP::Guild::get_system_channel() {
    if (!system_channel.has_value() && system_channel_id.has_value()) {
        system_channel.emplace(system_channel_id.value(), get_token());
    }
    return system_channel;
}
