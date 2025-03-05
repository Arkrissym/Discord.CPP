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
    default_member_permissions = Permissions(get_or_else<std::string>(data, "default_member_permissions", "0"));
    nsfw = get_or_else<bool>(data, "nsfw", false);
    version = data.at("version").get<std::string>();

    if (has_value(data, "integration_types")) {
        for (const json& integration_type : data.at("integration_types")) {
            integration_types.push_back(static_cast<DiscordCPP::ApplicationCommand::IntegrationType>(integration_type.get<int>()));
        }
    }

    if (has_value(data, "contexts")) {
        for (const json& context : data.at("contexts")) {
            contexts.push_back(static_cast<DiscordCPP::ApplicationCommand::ContextType>(context.get<int>()));
        }
    }

    if (has_value(data, "handler")) {
        handler = static_cast<DiscordCPP::ApplicationCommand::HandlerType>(data.at("handler").get<int>());
    }

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
    data["nsfw"] = nsfw;
    data["default_member_permissions"] = default_member_permissions.to_json();

    for (auto& integration_type : integration_types) {
        data["integration_types"].push_back(integration_type);
    }

    for (auto& context : contexts) {
        data["contexts"].push_back(context);
    }

    if (handler.has_value()) {
        data["handler"] = handler.value();
    }

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
