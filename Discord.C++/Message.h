#pragma once

#include "DiscordObject.h"
#include "Embed.h"
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
    User author;
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
    // vector<Reaction> reactions;
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

    /// @return The id of the Channel the message was sent in
    std::string get_channel_id() { return channel_id; }
    /// @return The Channel the message was sent in
    TextChannel get_channel();
    /// @return The id of the Guild the message was sent in
    std::optional<std::string> get_guild_id() { return guild_id; }
    /// @return The Guild the message was sent in
    std::optional<DiscordCPP::Guild> get_guild();
    /// @return The author of this message
    User get_author() { return author; }
    /// @return The content of this message
    std::string get_content() { return content; }
    /// @return When the message was created
    std::string get_timestamp() { return timestamp; }
    /// @return When the message was edited
    std::string get_edited_timestamp() { return edited_timestamp; }
    /// @return True, if this is a tts message
    bool is_tts() { return tts; }
    /// @return True, if everyone is mentioned
    bool is_mention_everyone() { return mention_everyone; }
    /// @return Array of mentioned users
    std::vector<User> get_mentions() { return mentions; }
    /// @return Array of embeds
    std::vector<Embed> get_embeds() { return embeds; }
    /// @return True, if this message has been pinned
    bool is_pinned() { return pinned; }
    /// @return The type of the messsage
    Type get_type() { return type; }
};

}  // namespace DiscordCPP
