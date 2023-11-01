#pragma once

#include <optional>

#include "InteractionData.h"
#include "Member.h"
#include "Message.h"
#include "static.h"

namespace DiscordCPP {

class Interaction : public DiscordObject {
   public:
    enum Type {
        PING = 1,
        APPLICATION_COMMAND = 2,
        MESSAGE_COMPONENT = 3,
        APPLICATION_COMMAND_AUTOCOMPLETE = 4,
        MODAL_SUBMIT = 5
    };

   private:
    /// A token used to respond to the interaction.
    std::string token;

    /// ID of the application this interaction is for.
    std::string application_id;
    /// The type of the invoked command.
    Type type;
    /// The command data payload.
    std::optional<InteractionData> data;
    /// The guild the interaction was sent from.
    std::optional<std::string> guild_id;
    /// The channel the interaction was sent from.
    std::optional<std::string> channel_id;
    /// The member that sent the interaction.
    std::optional<Member> member;
    /// The user that sent the interaction.
    std::optional<User> user;
    /// For components, the message they were attached to.
    std::optional<Message> message;
    /// The selected locale of the user who sent this interaction.
    std::optional<std::string> locale;
    /// The guild's preferred locale.
    std::optional<std::string> guild_locale;

   public:
    DLL_EXPORT Interaction(const json& data, const std::string& token);

    /// Send a Message as a reply to this interaction.
    DLL_EXPORT void reply(const std::string& content, const bool tts = false);
    /// Send an Embed as a reply to this interaction.
    DLL_EXPORT void reply(Embed embed);

    /// Update a previously sent reply for this interaction.
    DLL_EXPORT void update_reply(const std::string& content, const bool tts = false);
    /// Update a previously sent reply for this interaction.
    DLL_EXPORT void update_reply(Embed embed);

    /// @return ID of the application this interaction is for.
    DLL_EXPORT std::string get_application_id() { return application_id; }
    /// @return The type of the invoked command.
    DLL_EXPORT Type get_type() { return type; }
    /// @return The command data payload.
    DLL_EXPORT std::optional<InteractionData> get_data() { return data; }
    /// @return The guild the interaction was sent from.
    DLL_EXPORT std::optional<std::string> get_guild_id() { return guild_id; }
    /// @return The channel the interaction was sent from.
    DLL_EXPORT std::optional<std::string> get_channel_id() { return channel_id; }
    /// @return The member that sent the interaction.
    DLL_EXPORT std::optional<Member> get_member() { return member; }
    /// @return The user that sent the interaction.
    DLL_EXPORT std::optional<User> get_user() { return user; }
    /// @return For components, the message they were attached to.
    DLL_EXPORT std::optional<Message> get_message() { return message; }
    /// @return The selected locale of the user who sent this interaction.
    DLL_EXPORT std::optional<std::string> get_locale() { return locale; }
    /// @return The guild's preferred locale.
    DLL_EXPORT std::optional<std::string> get_guild_locale() { return guild_locale; }
};

}  // namespace DiscordCPP
