#include "Emoji.h"

DiscordCPP::Emoji::Emoji(const json& data, const std::string& token)
    : DiscordObject(token, data.at("id").get<std::string>()) {
    name = get_optional<std::string>(data, "name");

    if (has_value(data, "roles")) {
        for (json role : data.at("roles")) {
            role_ids.push_back(role.get<std::string>());
        }
    }
    if (has_value(data, "data")) {
        user = User(data.at("user"), get_token());
    }

    require_colons = get_or_else<bool>(data, "require_colons", false);
    managed = get_or_else<bool>(data, "managed", false);
    animated = get_or_else<bool>(data, "animated", false);
    available = get_or_else<bool>(data, "available", true);
}
