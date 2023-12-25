#pragma once
#include <cmath>
#include <optional>
#include <utility>
#include <vector>

#include "ApplicationCommandOption.h"
#include "Channel.h"
#include "DiscordObject.h"
#include "Guild.h"
#include "static.h"

namespace DiscordCPP {

class ApplicationCommand : public DiscordObject {
   public:
    enum Type {
        CHAT_INPUT = 1,
        USER = 2,
        MESSAGE = 3
    };

   private:
    /// The command type.
    Type type;
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
    std::vector<ApplicationCommandOptionVariant> options;
    // default_member_permissions
    /// Indicates wether the command is enabled in DMs. Defaults to true.
    bool dm_permission = true;
    /// Autoincrementing version identifier updated during substantial record changes.
    std::string version;

   public:
    DLL_EXPORT ApplicationCommand() = default;
    DLL_EXPORT ApplicationCommand(const json& data, const std::string& token);

    DLL_EXPORT json to_json();

    /// Delete this command.
    DLL_EXPORT void delete_command();

    /// @return The application id.
    DLL_EXPORT std::string get_application_id() { return application_id; }
    /// @return The guild of the command.
    DLL_EXPORT std::optional<std::string> get_guild_id() { return guild_id; }
    /// @return The command's name.
    DLL_EXPORT std::string get_name() { return name; }
    /// @return The command's description. Empty string for USER and MESSAGE commands.
    DLL_EXPORT std::string get_description() { return description; }
    /// @return The command type.
    DLL_EXPORT Type get_type() { return type; }
    /// @return Parameters for the command, max of 25.
    DLL_EXPORT std::vector<ApplicationCommandOptionVariant> get_options() { return options; }
    /// @return Indicates wether the command is enabled in DMs. Defaults to true.
    DLL_EXPORT bool has_dm_permission() { return dm_permission; }
    /// @return Autoincrementing version identifier updated during substantial record changes.
    DLL_EXPORT std::string get_version() { return version; }

    /// Set the guild of the command.
    DLL_EXPORT void set_guild_id(std::string guild_id) { this->guild_id.emplace(guild_id); }
    /// Set the command's name.
    DLL_EXPORT void set_name(std::string name) { this->name = std::move(name); }
    /// Set the command's description. Empty string for USER and MESSAGE commands.
    DLL_EXPORT void set_description(std::string description) { this->description = std::move(description); }
    /// Set the command type.
    DLL_EXPORT void set_type(Type type) { this->type = type; }
    /// Add parameters for the command, max of 25.
    DLL_EXPORT void add_option(ApplicationCommandOptionVariant option) { options.push_back(option); }
    /// Set indicator wether the command is enabled in DMs.
    DLL_EXPORT void set_dm_permission(bool dm_permission) { this->dm_permission = dm_permission; }
};
}  // namespace DiscordCPP