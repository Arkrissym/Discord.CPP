#include "ApplicationCommand.h"

#include <variant>

DiscordCPP::ApplicationCommand::ApplicationCommand(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token, data.at("id").get<std::string>()) {
    type = static_cast<DiscordCPP::ApplicationCommand::Type>(get_or_else<int>(data, "type", CHAT_INPUT));
    application_id = data.at("application_id").get<std::string>();

    if (has_value(data, "guild_id")) {
        guild_id = std::make_optional<std::string>(data.at("guild_id").get<std::string>());
    }

    name = data.at("name").get<std::string>();
    description = data.at("description").get<std::string>();
    dm_permission = get_or_else<bool>(data, "dm_permission", true);
    version = data.at("version").get<std::string>();

    if (has_value(data, "options")) {
        for (const json& option : data.at("options")) {
            options.push_back(DiscordCPP::ApplicationCommandOptionHelper::application_command_option_from_json(option));
        }
    }
}

json DiscordCPP::ApplicationCommand::to_json() {
    json data;
    data["type"] = type;
    data["name"] = name;
    data["description"] = description;
    data["dm_permission"] = dm_permission;

    for (auto& option : options) {
        data["options"].push_back(std::visit([](auto v) { return v.to_json(); }, option));
    }

    return data;
}

void DiscordCPP::ApplicationCommand::delete_command() {
    std::string url = "/applications/" + application_id;
    if (guild_id.has_value()) {
        url += "/guilds/" + guild_id.value();
    }
    url += "/commands/" + get_id();

    api_call(url, "DELETE");
}