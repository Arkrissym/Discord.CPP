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
