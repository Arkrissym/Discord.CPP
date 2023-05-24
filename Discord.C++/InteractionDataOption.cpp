#include "InteractionDataOption.h"

DiscordCPP::InteractionDataOption::InteractionDataOption(const json& data) {
    data.at("name").get_to<std::string>(name);
    type = static_cast<ApplicationCommandOption::Type>(data.at("type").get<int>());
}

DiscordCPP::InteractionDataOption* DiscordCPP::InteractionDataOption::from_json(const json& data) {
    switch (data.at("type").get<int>()) {
        case ApplicationCommandOption::Type::STRING:
            return (InteractionDataOption*)new InteractionDataStringOption(data);
        case ApplicationCommandOption::Type::INTEGER:
            return (InteractionDataOption*)new InteractionDataIntegerOption(data);
        case ApplicationCommandOption::Type::NUMBER:
            return (InteractionDataOption*)new InteractionDataNumberOption(data);
        case ApplicationCommandOption::Type::BOOLEAN:
            return (InteractionDataOption*)new InteractionDataBooleanOption(data);
        case ApplicationCommandOption::Type::SUB_COMMAND:
        case ApplicationCommandOption::Type::SUB_COMMAND_GROUP:
            return (InteractionDataOption*)new InteractionDataSubcommandOption(data);
        default:
            return new InteractionDataOption(data);
    }
}

DiscordCPP::InteractionDataOption* DiscordCPP::InteractionDataOption::copy() {
    switch (type) {
        case ApplicationCommandOption::Type::STRING:
            return (InteractionDataOption*)new InteractionDataStringOption(*(InteractionDataStringOption*)this);
        case ApplicationCommandOption::Type::INTEGER:
            return (InteractionDataOption*)new InteractionDataIntegerOption(*(InteractionDataIntegerOption*)this);
        case ApplicationCommandOption::Type::NUMBER:
            return (InteractionDataOption*)new InteractionDataNumberOption(*(InteractionDataNumberOption*)this);
        case ApplicationCommandOption::Type::BOOLEAN:
            return (InteractionDataOption*)new InteractionDataBooleanOption(*(InteractionDataBooleanOption*)this);
        case ApplicationCommandOption::Type::SUB_COMMAND:
        case ApplicationCommandOption::Type::SUB_COMMAND_GROUP:
            return (InteractionDataOption*)new InteractionDataSubcommandOption(*(InteractionDataSubcommandOption*)this);
        default:
            return new InteractionDataOption(*this);
    }
}

DiscordCPP::InteractionDataStringOption::InteractionDataStringOption(const json& data)
    : InteractionDataOption(data) {
    data.at("value").get_to<std::string>(value);
}

DiscordCPP::InteractionDataIntegerOption::InteractionDataIntegerOption(const json& data)
    : InteractionDataOption(data) {
    data.at("value").get_to<int>(value);
}

DiscordCPP::InteractionDataNumberOption::InteractionDataNumberOption(const json& data)
    : InteractionDataOption(data) {
    data.at("value").get_to<double>(value);
}

DiscordCPP::InteractionDataBooleanOption::InteractionDataBooleanOption(const json& data)
    : InteractionDataOption(data) {
    data.at("value").get_to<bool>(value);
}

DiscordCPP::InteractionDataSubcommandOption::InteractionDataSubcommandOption(const json& data)
    : InteractionDataOption(data) {
    for (json option : data.at("options")) {
        options.push_back(InteractionDataOption::from_json(data.at("options")));
    }
}

DiscordCPP::InteractionDataSubcommandOption::InteractionDataSubcommandOption(const InteractionDataSubcommandOption& other)
    : InteractionDataOption(other) {
    for (unsigned int i = 0; i < other.options.size(); i++) {
        options.push_back(other.options[i]->copy());
    }
}

DiscordCPP::InteractionDataSubcommandOption::~InteractionDataSubcommandOption() {
    for (unsigned int i = 0; i < options.size(); i++) {
        delete options[i];
    }
}
