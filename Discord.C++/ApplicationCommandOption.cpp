#include "ApplicationCommandOption.h"

DiscordCPP::ApplicationCommandOption* DiscordCPP::ApplicationCommandOption::from_json(const json& data) {
    Type type = static_cast<Type>(get_or_else<int>(data, "type", 1));
    switch (type) {
        case SUB_COMMAND:
        case SUB_COMMAND_GROUP:
            return new ApplicationCommandSubcommand(data);
        case STRING:
            return new ApplicationCommandValueOption(data);
        case INTEGER:
            return new ApplicationCommandIntegerOption(data);
        case NUMBER:
            return new ApplicationCommandNumberOption(data);
        case CHANNEL:
            return new ApplicationCommandChannelOption(data);
        default:
            return new ApplicationCommandOption(data);
    }
}

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

DiscordCPP::ApplicationCommandOption* DiscordCPP::ApplicationCommandOption::copy() {
    switch (type) {
        case SUB_COMMAND:
        case SUB_COMMAND_GROUP:
            return new ApplicationCommandSubcommand(*(ApplicationCommandSubcommand*)this);
        case STRING:
            return new ApplicationCommandValueOption(*(ApplicationCommandValueOption*)this);
        case INTEGER:
            return new ApplicationCommandIntegerOption(*(ApplicationCommandIntegerOption*)this);
        case NUMBER:
            return new ApplicationCommandNumberOption(*(ApplicationCommandNumberOption*)this);
        case CHANNEL:
            return new ApplicationCommandChannelOption(*(ApplicationCommandChannelOption*)this);
        default:
            return new ApplicationCommandOption(*this);
    }
}

DiscordCPP::ApplicationCommandSubcommand::ApplicationCommandSubcommand() {
    type = SUB_COMMAND;
}

DiscordCPP::ApplicationCommandSubcommand::ApplicationCommandSubcommand(const json& data)
    : ApplicationCommandOption(data) {
    if (has_value(data, "options")) {
        for (const json& option : data.at("options")) {
            options.push_back(ApplicationCommandOption::from_json(option));
        }
    }
}

DiscordCPP::ApplicationCommandSubcommand::ApplicationCommandSubcommand(const ApplicationCommandSubcommand& other)
    : ApplicationCommandOption(other) {
    for (ApplicationCommandOption* option : other.options) {
        options.push_back(option->copy());
    }
}

DiscordCPP::ApplicationCommandSubcommand::~ApplicationCommandSubcommand() {
    for (ApplicationCommandOption* option : options) {
        delete option;
    }
}

json DiscordCPP::ApplicationCommandSubcommand::to_json() {
    json data = ApplicationCommandOption::to_json();
    for (auto option : options) {
        data["options"].push_back(option->to_json());
    }
    return data;
}

DiscordCPP::ApplicationCommandChannelOption::ApplicationCommandChannelOption() {
    type = CHANNEL;
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
    type = STRING;
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
    type = INTEGER;
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
    type = NUMBER;
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