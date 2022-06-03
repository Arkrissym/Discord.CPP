#include "InteractionData.h"

DiscordCPP::InteractionData::InteractionData(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token) {
    data.at("id").get_to<std::string>(id);
    data.at("name").get_to<std::string>(name);
    type = static_cast<ApplicationCommand::Type>(data.at("type").get<int>());
    // resolved
    // options
    guild_id = get_optional<std::string>(data, "guild_id");
    custom_id = get_optional<std::string>(data, "custom_id");
    // component_type
    // values
    target_id = get_optional<std::string>(data, "target_id");
    // components
}
