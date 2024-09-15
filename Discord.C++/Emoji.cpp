#include "Emoji.h"

DiscordCPP::Emoji::Emoji(const json& data, const std::string& token)
    : DiscordObject(token, get_or_else<std::string>(data, "id", "")) {
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

DiscordCPP::Emoji DiscordCPP::Emoji::by_name(const std::string& name) {
    Emoji emoji = Emoji();
    emoji.name = name;

    return emoji;
}

std::string DiscordCPP::Emoji::url_encode() {
    std::string url_encoded = urlencode(name.value_or(""));
    if (get_id().has_value()) {
        url_encoded.append(urlencode(":" + get_id().value()));
    }
    return url_encoded;
}

std::optional<std::string> DiscordCPP::Emoji::get_id() {
    if (DiscordCPP::DiscordObject::get_id() == "") {
        return {};
    }

    return DiscordCPP::DiscordObject::get_id();
}
