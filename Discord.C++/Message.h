#pragma once

#include <vector>

#include "DiscordObject.h"
#include "Embed.h"
#include "Emoji.h"
#include "MessageReaction.h"
#include "User.h"

namespace DiscordCPP {

class TextChannel;
class Guild;

class Message : public DiscordObject {
   public:
    enum Type {
        DEFAULT,
        RECIPIENT_ADD,
        RECIPIENT_REMOVE,
        CALL,
        CHANNEL_NAME_CHANGE,
        CHANNEL_ICON_CHANGE,
        CHANNEL_PINNED_MESSAGE,
        GUILD_MEMBER_JOIN
    };

   private:
    /// The channel the message was sent in
    std::string channel_id;
    TextChannel* channel = nullptr;
    /// The guild the message was sent in
    std::optional<std::string> guild_id;
    Guild* guild = nullptr;
    /// The author of this message
    std::optional<User> author;
    /// The content of this message
    std::string content;
    // ISO8601 timestamp
    /// when the message was created
    std::string timestamp;
    // ISO8601 timestamp
    /// when the message was edited
    std::string edited_timestamp;
    /// true, if this is a tts message
    bool tts;
    /// true, if everyone is mentioned
    bool mention_everyone;
    /// array of mentioned users
    std::vector<User> mentions;
    // vector<Role> mention_roles;
    // vector<Attachments> attachments;
    /// Array of embeds
    std::vector<Embed> embeds;
    /// Array of reactions
    std::vector<MessageReaction> reactions;
    /// true, if this message has been pinned
    bool pinned;
    /// the webhook id, if the message was generated by a webhook
    std::string webhook_id;  // snowflake
    /// the type of the messsage
    Type type;
    // MessageAcivity activity;
    // MessageApplication application;

   public:
    DLL_EXPORT Message(const json& data, const std::string& token);
    DLL_EXPORT Message(const Message& old);
    DLL_EXPORT Message() = default;
    DLL_EXPORT ~Message();

    /// Edit this message
    DLL_EXPORT Message edit(const std::string& content);
    /// Delete this message
    DLL_EXPORT void delete_msg();
    /// Send a reply to this message.
    DLL_EXPORT Message reply(const std::string& content, const bool tts = false);
    /// Send an Embed as reply to this message.
    DLL_EXPORT Message reply(Embed embed);
    /// Crosspost this message to another channel
    DLL_EXPORT Message crosspost(TextChannel channel);
    /// Add a reaction
    DLL_EXPORT void add_reaction(Emoji emoji);
    /// Remove own reaction
    DLL_EXPORT void remove_reaction(Emoji emoji);
    /// Remove another user's reaction
    DLL_EXPORT void remove_user_reaction(Emoji emoji, User user);
    /// Remove all reactions
    DLL_EXPORT void remove_all_reactions();
    /// Remove all reactions by emoji
    DLL_EXPORT void remove_all_reactions(Emoji emoji);

    /// @return The id of the Channel the message was sent in
    DLL_EXPORT std::string get_channel_id() { return channel_id; }
    /// @return The Channel the message was sent in
    DLL_EXPORT TextChannel get_channel();
    /// @return The id of the Guild the message was sent in
    DLL_EXPORT std::optional<std::string> get_guild_id() { return guild_id; }
    /// @return The Guild the message was sent in
    DLL_EXPORT std::optional<Guild> get_guild();
    /// @return The author of this message
    DLL_EXPORT std::optional<User> get_author() { return author; }
    /// @return The content of this message
    DLL_EXPORT std::string get_content() { return content; }
    /// @return When the message was created
    DLL_EXPORT std::string get_timestamp() { return timestamp; }
    /// @return When the message was edited
    DLL_EXPORT std::string get_edited_timestamp() { return edited_timestamp; }
    /// @return True, if this is a tts message
    DLL_EXPORT bool is_tts() { return tts; }
    /// @return True, if everyone is mentioned
    DLL_EXPORT bool is_mention_everyone() { return mention_everyone; }
    /// @return Array of mentioned users
    DLL_EXPORT std::vector<User> get_mentions() { return mentions; }
    /// @return Array of embeds
    DLL_EXPORT std::vector<Embed> get_embeds() { return embeds; }
    /// Array of reactions
    DLL_EXPORT std::vector<MessageReaction> get_reactions() { return reactions; }
    /// @return True, if this message has been pinned
    DLL_EXPORT bool is_pinned() { return pinned; }
    /// @return The type of the messsage
    DLL_EXPORT Type get_type() { return type; }
};

}  // namespace DiscordCPP
