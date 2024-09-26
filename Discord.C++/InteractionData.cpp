#include "InteractionData.h"

DiscordCPP::InteractionData::InteractionData(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token, data.at("id").get<std::string>()),
      name(data.at("name").get<std::string>()),
      type(static_cast<ApplicationCommand::Type>(data.at("type").get<int>())) {
    if (has_value(data, "options")) {
        for (const json& option : data.at("options")) {
            options.push_back(InteractionDataOptionHelper::interaction_data_option_from_json(option));
        }
    }
    guild_id = get_optional<std::string>(data, "guild_id");
    if (has_value(data, "resolved")) {
        resolved_data = InteractionResolvedData(data.at("resolved"), guild_id.value_or(""), token);
    }
    custom_id = get_optional<std::string>(data, "custom_id");
    target_id = get_optional<std::string>(data, "target_id");
    // component_type
    // values
    // components
}
