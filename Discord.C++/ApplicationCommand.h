#pragma once
#include <optional>
#include <vector>
#include "ApplicationCommandOption.h"
#include "Channel.h"
#include "DiscordObject.h"
#include "Guild.h"
#include "static.h"

namespace DiscordCPP {

class ApplicationCommand : public DiscordObject {
   public:
    /// The command type.
    enum Type {
        CHAT_INPUT = 1,
        USER = 2,
        MESSAGE = 3
    } type;
    /// The application id.
    std::string application_id;
    /// The guild of the command.
    std::optional<std::string> guild_id;
    /// The command's name.
    std::string name;
    // name_localizations
    /// The command's description. Empty string for USER and MESSAGE commands.
    std::string description;
    // description_localizations
    /// Parameters for the command, max of 25.
    std::vector<ApplicationCommandOption*> options;
    // default_member_permissions
    /// Indicates wether the command is enabled in DMs. Defaults to true.
    bool dm_permission = true;
    /// Autoincrementing version identifier updated during substantial record changes.
    std::string version;

    DLL_EXPORT ApplicationCommand() {}
    DLL_EXPORT ApplicationCommand(const json& data, const std::string& token);
    DLL_EXPORT ApplicationCommand(const ApplicationCommand& other);
    DLL_EXPORT ~ApplicationCommand();

    DLL_EXPORT json to_json();

    /// Delete this command.
    DLL_EXPORT void delete_command();
};
}  // namespace DiscordCPP