#include "ApplicationCommandOption.h"

#include <variant>

DiscordCPP::ApplicationCommandOption::ApplicationCommandOption(const json& data) {
    data.at("name").get_to<std::string>(name);
    data.at("description").get_to<std::string>(description);
    type = static_cast<Type>(get_or_else<int>(data, "type", 1));
    required = get_or_else<bool>(data, "required", false);
}

json DiscordCPP::ApplicationCommandOption::to_json() {
    json data;
    data["name"] = name;
    data["type"] = static_cast<int>(type);
    data["description"] = description;
    if (type >= 2) {
        data["required"] = required;
    }
    return data;
}

DiscordCPP::ApplicationCommandSubcommand::ApplicationCommandSubcommand() {
    set_type(SUB_COMMAND);
}

DiscordCPP::ApplicationCommandSubcommand::ApplicationCommandSubcommand(const json& data)
    : ApplicationCommandOption(data) {
    if (has_value(data, "options")) {
        for (const json& option : data.at("options")) {
            options.push_back(ApplicationCommandOptionHelper::application_command_option_from_json(option));
        }
    }
}

json DiscordCPP::ApplicationCommandSubcommand::to_json() {
    json data = ApplicationCommandOption::to_json();
    for (auto& option : options) {
        data["options"].push_back(std::visit([](auto v) { return v.to_json(); }, option));
    }
    return data;
}

void DiscordCPP::ApplicationCommandSubcommand::add_option(const ApplicationCommandOptionVariant& option) {
    options.push_back(option);
}

DiscordCPP::ApplicationCommandChannelOption::ApplicationCommandChannelOption() {
    set_type(CHANNEL);
}

DiscordCPP::ApplicationCommandChannelOption::ApplicationCommandChannelOption(const json& data)
    : ApplicationCommandOption(data) {
    for (const json& channel_type : data.at("channel_types")) {
        channel_types.push_back(static_cast<Channel::Type>(channel_type.get<int>()));
    }
}

json DiscordCPP::ApplicationCommandChannelOption::to_json() {
    json data = ApplicationCommandOption::to_json();
    for (auto channel_type : channel_types) {
        data["channel_types"].push_back(static_cast<int>(channel_type));
    }
    return data;
}

DiscordCPP::ApplicationCommandValueOption::ApplicationCommandValueOption() {
    set_type(STRING);
}

DiscordCPP::ApplicationCommandValueOption::ApplicationCommandValueOption(const json& data)
    : ApplicationCommandOption(data) {
    autocomplete = get_or_else<bool>(data, "autocomplete", false);
}

json DiscordCPP::ApplicationCommandValueOption::to_json() {
    json data = ApplicationCommandOption::to_json();
    data["autocomplete"] = autocomplete;
    return data;
}

DiscordCPP::ApplicationCommandIntegerOption::ApplicationCommandIntegerOption() {
    set_type(INTEGER);
}

DiscordCPP::ApplicationCommandIntegerOption::ApplicationCommandIntegerOption(const json& data)
    : ApplicationCommandValueOption(data) {
    data.at("min_value").get_to<int>(min_value);
    data.at("max_value").get_to<int>(max_value);
}

json DiscordCPP::ApplicationCommandIntegerOption::to_json() {
    json data = ApplicationCommandValueOption::to_json();
    data["min_value"] = min_value;
    data["max_value"] = max_value;
    return data;
}

DiscordCPP::ApplicationCommandNumberOption::ApplicationCommandNumberOption() {
    set_type(NUMBER);
}

DiscordCPP::ApplicationCommandNumberOption::ApplicationCommandNumberOption(const json& data)
    : ApplicationCommandValueOption(data) {
    data.at("min_value").get_to<double>(min_value);
    data.at("max_value").get_to<double>(max_value);
}

json DiscordCPP::ApplicationCommandNumberOption::to_json() {
    json data = ApplicationCommandValueOption::to_json();
    data["min_value"] = min_value;
    data["max_value"] = max_value;
    return data;
}

DiscordCPP::ApplicationCommandOptionVariant DiscordCPP::ApplicationCommandOptionHelper::application_command_option_from_json(const json& data) {
    ApplicationCommandOption::Type type = static_cast<ApplicationCommandOption::Type>(get_or_else<int>(data, "type", 1));
    switch (type) {
        case ApplicationCommandOption::Type::SUB_COMMAND:
        case ApplicationCommandOption::Type::SUB_COMMAND_GROUP:
            return ApplicationCommandSubcommand(data);
        case ApplicationCommandOption::Type::STRING:
            return ApplicationCommandValueOption(data);
        case ApplicationCommandOption::Type::INTEGER:
            return ApplicationCommandIntegerOption(data);
        case ApplicationCommandOption::Type::NUMBER:
            return ApplicationCommandNumberOption(data);
        case ApplicationCommandOption::Type::CHANNEL:
            return ApplicationCommandChannelOption(data);
        default:
            return ApplicationCommandOption(data);
    }
}
