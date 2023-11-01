#pragma once
#include <utility>

#include "Channel.h"

namespace DiscordCPP {

class Message;
class Embed;

class TextChannel : public Channel {
   private:
    /// the topic of the channel
    std::string topic;
    /// true if nsfw is enabled
    bool nsfw;
    /// the id of the last message
    std::string last_message_id;
    /// when the last message was pinned
    std::string last_pin_timestamp;  // ISO8601 timestamp
    /// amount of seconds a User has to wait before sending another message(0-120). Bots, as well as Users with MANAGE_MESSAGES or MANAGE_CHANNELS Permissions are unaffected.
    int rate_limit_per_user = 0;

   public:
    DLL_EXPORT TextChannel(const json& data, const std::string& token);
    DLL_EXPORT TextChannel(const std::string& id, const std::string& token);
    DLL_EXPORT TextChannel() = default;

    /// Send a Message to this channel.
    DLL_EXPORT Message send(const std::string& content, const bool tts = false);
    /// Send an Embed to this channel.
    DLL_EXPORT Message send(Embed embed);

    /// Get messages from channel history
    DLL_EXPORT std::vector<std::shared_ptr<Message>> history(const int limit = 100, const std::string& before = "", const std::string& after = "", const std::string& around = "");

    /// Delete multiple messages from this channel
    DLL_EXPORT void delete_messages(const std::vector<std::shared_ptr<Message>>& messages);

    /// @return The topic of the channel
    DLL_EXPORT std::string get_topic() { return topic; }
    /// @return True if nsfw is enabled
    DLL_EXPORT bool is_nsfw() { return nsfw; }
    /// @return The id of the last message
    DLL_EXPORT std::string get_last_message_id() { return last_message_id; }
    /// @return When the last message was pinned
    DLL_EXPORT std::string get_last_pin_timestamp() { return last_pin_timestamp; }
    /// @return Amount of seconds a User has to wait before sending another message(0-120).
    DLL_EXPORT int get_rate_limit_per_user() { return rate_limit_per_user; }

    DLL_EXPORT void _set_last_message_id(std::string message_id) { last_message_id = message_id; }
    DLL_EXPORT void _set_last_pin_timestamp(std::string timestamp) { last_pin_timestamp = timestamp; }
};

}  // namespace DiscordCPP
