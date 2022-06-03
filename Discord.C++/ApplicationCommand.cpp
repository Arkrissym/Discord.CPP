#include "ApplicationCommand.h"

DiscordCPP::ApplicationCommand::ApplicationCommand(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token) {
    id = data.at("id").get<std::string>();
    type = static_cast<DiscordCPP::ApplicationCommand::Type>(get_or_else<int>(data, "type", 1));
    application_id = data.at("application_id").get<std::string>();

    if (has_value(data, "guild_id")) {
        guild_id = std::make_optional<std::string>(data.at("guild_id").get<std::string>());
    }

    name = data.at("name").get<std::string>();
    description = data.at("description").get<std::string>();
    version = data.at("version").get<std::string>();

    if (has_value(data, "options")) {
        for (json option : data.at("options")) {
            options.push_back(DiscordCPP::ApplicationCommandOption::from_json(option));
        }
    }
}

json DiscordCPP::ApplicationCommand::to_json() {
    json data;
    data["type"] = type;
    data["name"] = name;
    data["description"] = description;

    for (auto option : options) {
        data["options"].push_back(option.to_json());
    }

    return data;
}

void DiscordCPP::ApplicationCommand::delete_command() {
    std::string url = "/applications/" + application_id;
    if (guild_id.has_value()) {
        url += "/guilds/" + guild_id.value();
    }
    url += "/commands/" + id;

    api_call(url, "DELETE");
}