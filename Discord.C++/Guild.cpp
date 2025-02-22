#include "Guild.h"

#include <string>
#include <vector>

#include "ChannelHelper.h"
#include "Emoji.h"
#include "Exceptions.h"
#include "Role.h"
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

    if (has_value(data, "roles")) {
        for (const json& role : data.at("roles")) {
            roles.push_back(Role(role, get_id(), token));
        }
    }

    if (has_value(data, "emojis")) {
        for (const json& emoji : data.at("emojis")) {
            emojis.push_back(Emoji(emoji, get_token()));
        }
    }

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
            channels.push_back(ChannelHelper::channel_from_json(client, channel, token));
        }
    }

    // presences
}

DiscordCPP::Guild::Guild(Discord* client, const std::string& id, const std::string& token)
    : DiscordCPP::DiscordObject(token) {
    std::string url = "/guilds/" + id;

    *this = Guild(client, api_call("/guilds/" + id), token);
}

void DiscordCPP::Guild::_add_channel(ChannelVariant channel) {
    std::lock_guard<std::mutex> lock(mutex_holder.mutex);
    channels.push_back(channel);
}

void DiscordCPP::Guild::_update_channel(ChannelVariant channel) {
    std::lock_guard<std::mutex> lock(mutex_holder.mutex);
    std::string channel_id = ChannelHelper::get_channel_id(channel);
    for (size_t i = 0; i < channels.size(); i++) {
        if (ChannelHelper::get_channel_id(channels[i]) == channel_id) {
            channels.erase(channels.begin() + i);
            channels.push_back(channel);
            break;
        }
    }
}

void DiscordCPP::Guild::_remove_channel(const std::string& channel_id) {
    std::lock_guard<std::mutex> lock(mutex_holder.mutex);
    for (size_t i = 0; i < channels.size(); i++) {
        if (ChannelHelper::get_channel_id(channels[i]) == channel_id) {
            channels.erase(channels.begin() + i);
            break;
        }
    }
}

void DiscordCPP::Guild::_add_member(Member member) {
    std::lock_guard<std::mutex> lock(mutex_holder.mutex);
    members.push_back(member);
    member_count += 1;
}

void DiscordCPP::Guild::_update_member(Member member) {
    std::lock_guard<std::mutex> lock(mutex_holder.mutex);
    for (size_t i = 0; i < members.size(); i++) {
        if (members[i].get_id() == member.get_id()) {
            members.erase(members.begin() + i);
            members.push_back(member);
        }
    }
}

void DiscordCPP::Guild::_remove_member(const std::string& member_id) {
    std::lock_guard<std::mutex> lock(mutex_holder.mutex);
    for (size_t i = 0; i < members.size(); i++) {
        if (members[i].get_id() == member_id) {
            members.erase(members.begin() + i);
            member_count -= 1;
        }
    }
}

void DiscordCPP::Guild::_update_emojis(std::vector<Emoji> emojis) {
    std::lock_guard<std::mutex> lock(mutex_holder.mutex);
    this->emojis = emojis;
}

void DiscordCPP::Guild::_add_role(Role role) {
    std::lock_guard<std::mutex> lock(mutex_holder.mutex);
    roles.push_back(role);
}

void DiscordCPP::Guild::_update_role(Role role) {
    std::lock_guard<std::mutex> lock(mutex_holder.mutex);
    for (size_t i = 0; i < roles.size(); i++) {
        if (roles[i].get_id() == role.get_id()) {
            roles.erase(roles.begin() + i);
            roles.push_back(role);
        }
    }
}

void DiscordCPP::Guild::_remove_role(const std::string& role_id) {
    std::lock_guard<std::mutex> lock(mutex_holder.mutex);
    for (size_t i = 0; i < roles.size(); i++) {
        if (roles[i].get_id() == role_id) {
            roles.erase(roles.begin() + i);
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

DiscordCPP::Permissions DiscordCPP::Guild::get_member_permissions(const Member& member) {
    if (owner_id.has_value() && owner_id.value() == member.get_id()) {
        return Permissions::All();
    }

    std::map<std::string, Permissions> permission_map;
    for (const Role& role : roles) {
        permission_map.insert({role.get_id(), role.get_permissions()});
    }

    Permissions permissions;
    for (std::string role : member.get_roles()) {
        if (permission_map.find(role) == permission_map.end()) {
            throw DiscordException("unknown role " + role + " for member " + member.get_id() + " in guild " + get_id());
        }

        if (permission_map.at(role).has_permission(Permissions::ADMINISTRATOR)) {
            return Permissions::All();
        }

        permissions.add(permission_map.at(role));
    }

    return permissions;
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
