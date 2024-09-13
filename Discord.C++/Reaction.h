#pragma once

#include <optional>
#include <string>
#include <vector>

#include "DiscordObject.h"
#include "Emoji.h"
#include "Guild.h"
#include "Member.h"
#include "Message.h"
#include "TextChannel.h"
#include "Threadpool.h"
#include "User.h"

namespace DiscordCPP {
class Reaction : public BaseDiscordObject {
   public:
    enum Type {
        NORMAL = 0,
        BURST = 1
    };

   private:
    /// id of the user
    std::string user_id;
    /// the user who sent the reaction
    std::optional<User> user;
    /// id of the channel
    std::string channel_id;
    /// the channel of the message
    std::optional<TextChannel> channel;
    /// id of the message
    std::string message_id;
    /// the message of this reaction
    std::optional<Message> message;
    /// id of the guild
    std::optional<std::string> guild_id;
    /// the guild of the channel
    std::optional<Guild> guild;
    /// the guild member who reacted
    std::optional<Member> member;
    /// emoji used to react
    Emoji emoji;
    /// id of the message author
    std::optional<std::string> message_author_id;
    /// the message author
    std::optional<User> message_author;
    /// true if this is a super reaction
    bool burst;
    /// colors used for super reaction im #rrggbb format
    std::vector<std::string> burst_colors;
    /// the type of the reaction
    Type type;

   public:
    DLL_EXPORT Reaction(const json& data, const std::string& token);

    /// @return id of the user
    DLL_EXPORT std::string get_user_id() { return user_id; }
    /// @return the user who sent the reaction
    DLL_EXPORT User get_user();
    /// @return id of the channel
    DLL_EXPORT std::string get_channel_id() { return channel_id; }
    /// @return the channel of the message
    DLL_EXPORT TextChannel get_channel();
    /// @return id of the message
    DLL_EXPORT std::string get_message_id() { return message_id; }
    /// @return the message of this reaction
    DLL_EXPORT Message get_message();
    /// @return id of the guild
    DLL_EXPORT std::optional<std::string> get_guild_id() { return guild_id; }
    /// @return the guild of the channel
    DLL_EXPORT std::optional<Guild> get_guild();
    /// @return the guild member who reacted
    DLL_EXPORT std::optional<Member> get_member() { return member; }
    /// @return emoji used to react
    DLL_EXPORT Emoji get_emoji() { return emoji; }
    /// @return id of the message author
    DLL_EXPORT std::optional<std::string> get_message_author_id() { return message_author_id; }
    /// @return the message author
    DLL_EXPORT std::optional<User> get_message_author();
    /// @return true if this is a super reaction
    DLL_EXPORT bool is_burst() { return burst; }
    /// @return colors used for super reaction im #rrggbb format
    DLL_EXPORT std::vector<std::string> get_burst_colors() { return burst_colors; }
    /// @return the type of the reaction
    DLL_EXPORT Type get_type() { return type; }
};
}  // namespace DiscordCPP
