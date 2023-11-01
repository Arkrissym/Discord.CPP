#include "InteractionData.h"

DiscordCPP::InteractionData::InteractionData(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token, data.at("id").get<std::string>()) {
    data.at("name").get_to<std::string>(name);
    type = static_cast<ApplicationCommand::Type>(data.at("type").get<int>());
    if (has_value(data, "resolved")) {
        resolved_data = InteractionResolvedData(data.at("resolved"), token);
    }
    if (has_value(data, "options")) {
        for (json option : data.at("options")) {
            options.push_back(InteractionDataOption::from_json(option));
        }
    }
    guild_id = get_optional<std::string>(data, "guild_id");
    custom_id = get_optional<std::string>(data, "custom_id");
    target_id = get_optional<std::string>(data, "target_id");
    // component_type
    // values
    // components
}

DiscordCPP::InteractionData::InteractionData(const InteractionData& other)
    : DiscordCPP::DiscordObject(other) {
    name = other.name;
    type = other.type;
    resolved_data = other.resolved_data;
    for (unsigned int i = 0; i < other.options.size(); i++) {
        options.push_back(other.options[i]->copy());
    }
    guild_id = other.guild_id;
    custom_id = other.custom_id;
    // component_type
    // values
    target_id = other.target_id;
    // components
}

DiscordCPP::InteractionData::~InteractionData() {
    for (unsigned int i = 0; i < options.size(); i++) {
        delete options[i];
    }
}
