#pragma once

#include <optional>
#include <vector>
#include "ApplicationCommand.h"
#include "InteractionDataOption.h"

namespace DiscordCPP {

class InteractionData : public DiscordObject {
   public:
    /// name of the invoked command.
    std::string name;
    /// The type of the invoked command.
    ApplicationCommand::Type type;
    // resolved
    /// The parameters and values from the user.
    std::vector<InteractionDataOption*> options;
    /// The id of the guild the command is registered to.
    std::optional<std::string> guild_id;
    /// The custom id of the component.
    std::optional<std::string> custom_id;
    // component_type
    // values
    /// Id the of user or message targeted by a user or message command.
    std::optional<std::string> target_id;
    // components

    DLL_EXPORT InteractionData(const json& data, const std::string& token);
    DLL_EXPORT InteractionData(const InteractionData& other);
    DLL_EXPORT ~InteractionData();
};

}  // namespace DiscordCPP
