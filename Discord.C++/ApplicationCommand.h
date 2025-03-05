#pragma once
#include <optional>
#include <utility>
#include <vector>

#include "ApplicationCommandOption.h"
#include "DiscordObject.h"
#include "Permissions.h"
#include "static.h"

namespace DiscordCPP {

class ApplicationCommand : public DiscordObject {
   public:
    enum Type {
        CHAT_INPUT = 1,
        USER = 2,
        MESSAGE = 3,
        PRIMARY_ENTRY_POINT = 4
    };

    enum IntegrationType {
        GUILD_INSTALL = 0,
        USER_INSTALL = 1
    };

    enum ContextType {
        GUILD = 0,
        BOT_DM = 1,
        PRIVATE_CHANNEL = 2
    };

    enum HandlerType {
        APP_HANDLER = 1,
        DISCORD_LAUNCH_ACTIVITY = 2
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
    /// Set of permissions
    Permissions default_member_permissions;
    /// Indicates whether the command is age-restricted, defaults to false.
    bool nsfw = false;
    /// Installation contexts where the command is available, only for globally-scoped commands. Defaults to your app's configured contexts.
    std::vector<IntegrationType> integration_types;
    /// Interaction context(s) where the command can be used, only for globally-scoped commands. Defaults to all.
    std::vector<ContextType> contexts;
    /// Autoincrementing version identifier updated during substantial record changes.
    std::string version;
    /// Determines whether the interaction is handled by the app's interactions handler or by Discord. Use when type is PRIMARY_ENTRY_POINT.
    std::optional<HandlerType> handler;

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
    /// @return Set of permissions
    DLL_EXPORT Permissions get_default_member_permissions() { return default_member_permissions; }
    /// @return Indicates whether the command is age-restricted, defaults to false.
    DLL_EXPORT bool is_nsfw() { return nsfw; }
    /// @return Installation contexts where the command is available.
    DLL_EXPORT std::vector<IntegrationType> get_integration_types() { return integration_types; }
    /// @return Interaction context(s) where the command can be used.
    DLL_EXPORT std::vector<ContextType> get_contexts() { return contexts; }
    /// @return Autoincrementing version identifier updated during substantial record changes.
    DLL_EXPORT std::string get_version() { return version; }
    /// @return The handler type for PRIMARY_ENTRY_POINT.
    DLL_EXPORT std::optional<HandlerType> get_handler() { return handler; }

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
    /// Set the default member permissions required
    DLL_EXPORT void set_default_member_permissions(Permissions permissions) { default_member_permissions = permissions; }
    /// Set whether the command is age-restricted, defaults to false.
    DLL_EXPORT void set_nsfw(bool nsfw) { this->nsfw = nsfw; }
    /// Add installation context where the command is available.
    DLL_EXPORT void add_integration_types(IntegrationType integration_type) { integration_types.push_back(integration_type); }
    /// Add interaction context where the command can be used.
    DLL_EXPORT void add_contexts(ContextType context) { contexts.push_back(context); }
    /// Set the handler type for PRIMARY_ENTRY_POINT.
    DLL_EXPORT void set_handler(HandlerType handler) { this->handler.emplace(handler); }
};
}  // namespace DiscordCPP
