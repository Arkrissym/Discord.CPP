#include "Guild.h"

#include <cpprest/http_client.h>

#include "DMChannel.h"
#include "TextChannel.h"
#include "VoiceChannel.h"
#include "static.h"

DiscordCPP::Guild::Guild(Discord* client, const json& data, const std::string& token) : DiscordCPP::DiscordObject(token) {
    data.at("id").get_to<std::string>(id);
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

    if (has_value(data, "owner_id")) {
        owner = new User(data.at("owner_id").get<std::string>(), token);
    }

    if (has_value(data, "afk_channel_id")) {
        afk_channel = new VoiceChannel(client, data.at("afk_channel_id").get<std::string>(), token);
    }

    if (has_value(data, "embed_channel_id")) {
        embed_channel = new TextChannel(data.at("embed_channel_id").get<std::string>(), token);
    }

    //roles

    //emojis

    if (has_value(data, "features")) {
        for (json feature : data.at("features")) {
            features.push_back(feature.get<std::string>());
        }
    }

    if (has_value(data, "widget_channel_id")) {
        widget_channel = new Channel(data.at("widget_channel_id").get<std::string>(), token);
    }

    if (has_value(data, "system_channel_id")) {
        system_channel = new TextChannel(data.at("system_channel_id").get<std::string>(), token);
    }

    //voice_states

    if (has_value(data, "members")) {
        for (json member : data.at("members")) {
            members.push_back(new Member(member, token));
        }
    }

    if (has_value(data, "channels")) {
        for (json channel : data.at("channels")) {
            channels.push_back(Channel::from_json(client, channel, token));
        }
    }

    //presences
}

DiscordCPP::Guild::Guild(Discord* client, const std::string& id, const std::string& token) : DiscordCPP::DiscordObject(token) {
    std::string url = "/guilds/" + id;

    *this = Guild(client, api_call(url), token);
}

DiscordCPP::Guild::Guild(const Guild& old) {
    _token = old._token;
    id = old.id;
    name = old.name;
    icon = old.icon;
    splash = old.splash;
    if (old.owner != NULL)
        owner = new User(*old.owner);
    permissions = old.permissions;
    region = old.region;
    if (old.afk_channel != NULL)
        afk_channel = old.afk_channel->copy(*old.afk_channel);
    afk_timeout = old.afk_timeout;
    embed_enabled = old.embed_enabled;
    if (old.embed_channel != NULL)
        embed_channel = old.embed_channel->copy(*old.embed_channel);
    verification_level = old.verification_level;
    default_message_notifications = old.default_message_notifications;
    explicit_content_filter = old.explicit_content_filter;
    //roles
    //emojis
    features = old.features;
    mfa_level = old.mfa_level;
    application_id = old.application_id;
    widget_enabled = old.widget_enabled;
    if (old.widget_channel != NULL)
        widget_channel = old.widget_channel->copy(*old.widget_channel);
    if (old.system_channel != NULL)
        system_channel = old.system_channel->copy(*old.system_channel);
    joined_at = old.joined_at;
    large = old.large;
    unavailable = old.unavailable;
    member_count = old.member_count;
    //voice_states
    for (unsigned int i = 0; i < old.members.size(); i++) {
        members.push_back(new Member(*old.members[i]));
    }
    for (unsigned int i = 0; i < old.channels.size(); i++) {
        channels.push_back(old.channels[i]->copy(*old.channels[i]));
    }
    //presences
}

DiscordCPP::Guild::~Guild() {
    if (owner != NULL)
        delete owner;
    if (afk_channel != NULL)
        delete afk_channel;
    if (embed_channel != NULL)
        delete embed_channel;
    if (widget_channel != NULL)
        delete widget_channel;
    if (system_channel != NULL)
        delete system_channel;
    for (unsigned int i = 0; i < members.size(); i++) {
        delete members[i];
    }
    for (unsigned int i = 0; i < channels.size(); i++) {
        delete channels[i];
    }
}

void DiscordCPP::Guild::_add_channel(Channel* channel) {
    channels.push_back(channel);
}

void DiscordCPP::Guild::_update_channel(Channel* channel) {
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->id == channel->id) {
            delete channels[i];
            channels.erase(channels.begin() + i);
            channels.push_back(channel);
            break;
        }
    }
}

void DiscordCPP::Guild::_remove_channel(const std::string& channel_id) {
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->id == channel_id) {
            delete channels[i];
            channels.erase(channels.begin() + i);
            break;
        }
    }
}

void DiscordCPP::Guild::_add_member(Member* member) {
    members.push_back(member);
    member_count += 1;
}

void DiscordCPP::Guild::_update_member(Member* member) {
    for (size_t i = 0; i < members.size(); i++) {
        if (members[i]->id == member->id) {
            delete members[i];
            members.erase(members.begin() + i);
            members.push_back(member);
        }
    }
}

void DiscordCPP::Guild::_remove_member(const std::string& member_id) {
    for (size_t i = 0; i < members.size(); i++) {
        if (members[i]->id == member_id) {
            delete members[i];
            members.erase(members.begin() + i);
            member_count -= 1;
        }
    }
}

///@throws HTTPError
void DiscordCPP::Guild::leave() {
    std::string url = "/guilds/@me/guilds/" + id;
    api_call(url, "DEL");
}

///@throws HTTPError
void DiscordCPP::Guild::delete_guild() {
    std::string url = "/guilds/" + id;
    api_call(url, "DEL");
}

/**	@param[in]	user	User to kick
	@throws HTTPError
*/
void DiscordCPP::Guild::kick(const User& user) {
    std::string url = "/guilds/" + id + "/members/" + user.id;
    api_call(url, "DEL");
}

/**	@param[in]	user				User to ban
	@param[in]	reason				reason, why the User should be banned
	@param[in]	delete_message_days	(optional) number of days to delete messages from user (0-7)
	@throws HTTPError
*/
void DiscordCPP::Guild::ban(const User& user, const std::string& reason, const int delete_message_days) {
    std::string url = "/guilds/" + id + "/bans/" + user.id + "?delete-message-days=" + std::to_string(delete_message_days) + "&reason=" + urlencode(reason);
    api_call(url, "PUT");
}

/**	@param[in]	user	User to kick
	@throws HTTPError
*/
void DiscordCPP::Guild::unban(const User& user) {
    std::string url = "/guilds/" + id + "/bans/" + user.id;
    api_call(url, "DEL");
}
