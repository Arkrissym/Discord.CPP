#pragma once

#include <optional>
#include "InteractionData.h"
#include "Member.h"
#include "Message.h"
#include "static.h"

namespace DiscordCPP {

class Interaction : public DiscordObject {
   public:
    /// ID of the application this interaction is for.
    std::string application_id;
    /// The type of the invoked command.
    enum Type {
        PING = 1,
        APPLICATION_COMMAND = 2,
        MESSAGE_COMPONENT = 3,
        APPLICATION_COMMAND_AUTOCOMPLETE = 4,
        MODAL_SUBMIT = 5
    } type;
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
    /// A token used to respond to the interaction.
    std::string token;
    /// For components, the message they were attached to.
    std::optional<Message> message;
    /// The selected locale of the user who sent this interaction.
    std::optional<std::string> locale;
    /// The guild's preferred locale.
    std::optional<std::string> guild_locale;

    DLL_EXPORT Interaction(const json& data, const std::string& token);

    /// Send a Message as a reply to this interaction.
    DLL_EXPORT void reply(const std::string& content, const bool tts = false);
    /// Send an Embed as a reply to this interaction.
    DLL_EXPORT void reply(Embed embed);

    /// Update a previously sent reply for this interaction.
    DLL_EXPORT void update_reply(const std::string& content, const bool tts = false);
    /// Update a previously sent reply for this interaction.
    DLL_EXPORT void update_reply(Embed embed);
};

}  // namespace DiscordCPP
