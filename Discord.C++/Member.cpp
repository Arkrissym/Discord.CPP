#include "Member.h"

DiscordCPP::Member::Member(const json& data, const std::string& token) : User(data.at("user"), token) {
    nick = get_or_else<std::string>(data, "nick", "");
    deaf = get_or_else<bool>(data, "deaf", false);
    mute = get_or_else<bool>(data, "mute", false);

    if (has_value(data, "roles")) {
        for (const json& role : data.at("roles")) {
            role_ids.push_back(role.get<std::string>());
        }
    }
}

DiscordCPP::Member::operator std::string() {
    if (nick.empty())
        return get_username();
    else
        return nick;
}

void DiscordCPP::Member::add_role(const DiscordCPP::Role& role) {
    std::string url = "/guilds/" + role.get_guild_id() + "/members/" + get_id();

    role_ids.push_back(role.get_id());
    json data = {{"roles", role_ids}};

    api_call(url, "PATCH", data, "application/json", false);
}

void DiscordCPP::Member::remove_role(const DiscordCPP::Role& role) {
    std::string url = "/guilds/" + role.get_guild_id() + "/members/" + get_id();

    role_ids.erase(std::remove_if(role_ids.begin(),
                                  role_ids.end(),
                                  [role](std::string i) {
                                      return role.get_id() == i;
                                  }));
    json data = {{"roles", role_ids}};

    api_call(url, "PATCH", data, "application/json", false);
}
