#include "Role.h"

DiscordCPP::Role::Role(const json& data, const std::string& guild_id, const std::string& token)
    : DiscordCPP::DiscordObject(token, data.at("id").get<std::string>()) {
    this->guild_id = guild_id;
    name = data.at("name").get<std::string>();
    color = data.at("color").get<unsigned int>();
    hoist = data.at("hoist").get<bool>();
    icon = get_optional<std::string>(data, "icon");
    unicode_emoji = get_optional<std::string>(data, "unicode_emoji");
    position = data.at("position").get<unsigned int>();
    permissions = Permissions(data.at("permissions").get<std::string>());
    managed = data.at("managed").get<bool>();
    mentionable = data.at("mentionable").get<bool>();
    flags = data.at("flags").get<int>();
}

DiscordCPP::Role::Role(const std::string& id, const std::string& guild_id, const std::string& token)
    : DiscordCPP::DiscordObject(token, id) {
    std::string url = "/guilds/" + guild_id + "/roles/" + id;
    *this = Role(api_call(url), guild_id, token);
}

void DiscordCPP::Role::delete_role() {
    std::string url = "/guilds/" + guild_id + "/roles/" + get_id();
    api_call(url, "DELETE");
}
