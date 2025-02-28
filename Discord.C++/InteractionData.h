#pragma once

#include <optional>
#include <vector>

#include "ApplicationCommand.h"
#include "InteractionDataOption.h"
#include "InteractionResolvedData.h"

namespace DiscordCPP {

class InteractionData : public DiscordObject {
   private:
    /// name of the invoked command.
    std::string name;
    /// The type of the invoked command.
    ApplicationCommand::Type type;
    /// The resolved users, roles, channels, messages and/or attachments.
    std::optional<InteractionResolvedData> resolved_data;
    /// The parameters and values from the user.
    std::vector<InteractionDataOptionVariant> options;
    /// Id the of user or message targeted by a user or message command.
    std::optional<std::string> target_id;
    /// The custom id of the component.
    std::optional<std::string> custom_id;
    // component_type
    // values
    // components

   public:
    DLL_EXPORT InteractionData(const json& data, const std::string& token, const std::string& guild_id);

    /// @return name of the invoked command.
    DLL_EXPORT std::string get_name() { return name; }
    /// @return The type of the invoked command.
    DLL_EXPORT ApplicationCommand::Type get_type() { return type; }
    /// @return The resolved users, roles, channels, messages and/or attachments.
    DLL_EXPORT std::optional<InteractionResolvedData> get_resolved_data() { return resolved_data; }
    /// @return The parameters and values from the user.
    DLL_EXPORT std::vector<InteractionDataOptionVariant> get_options() { return options; }
    /// @return Id the of user or message targeted by a user or message command.
    DLL_EXPORT std::optional<std::string> get_target_id() { return target_id; }
    /// @return The custom id of the component.
    DLL_EXPORT std::optional<std::string> get_custom_id() { return custom_id; }
};

}  // namespace DiscordCPP
