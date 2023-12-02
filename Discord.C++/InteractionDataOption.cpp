#include "InteractionDataOption.h"

#include <variant>

#include "ApplicationCommandOption.h"

DiscordCPP::InteractionDataOption::InteractionDataOption(const json& data)
    : name(data.at("name").get<std::string>()),
      type(static_cast<ApplicationCommandOption::Type>(data.at("type").get<int>())) {
}

DiscordCPP::InteractionDataStringOption::InteractionDataStringOption(const json& data)
    : InteractionDataOption(data),
      value(data.at("value").get<std::string>()) {
}

DiscordCPP::InteractionDataIntegerOption::InteractionDataIntegerOption(const json& data)
    : InteractionDataOption(data),
      value(data.at("value").get<int>()) {
}

DiscordCPP::InteractionDataNumberOption::InteractionDataNumberOption(const json& data)
    : InteractionDataOption(data),
      value(data.at("value").get<double>()) {
}

DiscordCPP::InteractionDataBooleanOption::InteractionDataBooleanOption(const json& data)
    : InteractionDataOption(data),
      value(data.at("value").get<bool>()) {
}

DiscordCPP::InteractionDataSubcommandOption::InteractionDataSubcommandOption(const json& data)
    : InteractionDataOption(data) {
    for (const json& option : data.at("options")) {
        options.push_back(InteractionDataOptionHelper::interaction_data_option_from_json(option));
    }
}

DiscordCPP::InteractionDataOptionVariant DiscordCPP::InteractionDataOptionHelper::interaction_data_option_from_json(const json& data) {
    switch (data.at("type").get<int>()) {
        case ApplicationCommandOption::Type::STRING:
            return InteractionDataStringOption(data);
        case ApplicationCommandOption::Type::INTEGER:
            return InteractionDataIntegerOption(data);
        case ApplicationCommandOption::Type::NUMBER:
            return InteractionDataNumberOption(data);
        case ApplicationCommandOption::Type::BOOLEAN:
            return InteractionDataBooleanOption(data);
        case ApplicationCommandOption::Type::SUB_COMMAND:
        case ApplicationCommandOption::Type::SUB_COMMAND_GROUP:
            return InteractionDataSubcommandOption(data);
        default:
            return InteractionDataOption(data);
    }
}

std::string DiscordCPP::InteractionDataOptionHelper::get_interaction_data_option_name(const InteractionDataOptionVariant& variant) {
    return std::visit([](const auto& option) { return option.name; }, variant);
}

DiscordCPP::ApplicationCommandOption::Type DiscordCPP::InteractionDataOptionHelper::get_interaction_data_option_type(const InteractionDataOptionVariant& variant) {
    return std::visit([](const auto& option) { return option.type; }, variant);
}
