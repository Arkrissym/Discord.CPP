#pragma once
#include "Channel.h"

namespace DiscordCPP {

class Message;
class Embed;

class TextChannel : public Channel {
   public:
    ///the topic of the channel
    std::string topic;
    ///true if nsfw is enabled
    bool nsfw;
    ///the id of the last message
    std::string last_message_id;
    ///when the last message was pinned
    std::string last_pin_timestamp;  //ISO8601 timestamp
    ///amount of seconds a User has to wait before sending another message(0-120). Bots, as well as Users with MANAGE_MESSAGES or MANAGE_CHANNELS Permissions are unaffected.
    int rate_limit_per_user = 0;

    DLL_EXPORT TextChannel(const json& data, const std::string& token);
    DLL_EXPORT TextChannel(const std::string& id, const std::string& token);
    DLL_EXPORT TextChannel(const TextChannel& old);
    DLL_EXPORT TextChannel(){};

    ///Send a Message to this channel.
    DLL_EXPORT Message send(const std::string& content, const bool tts = false);
    DLL_EXPORT Message send(Embed embed);

    ///Get messages from channel history
    DLL_EXPORT std::vector<std::shared_ptr<Message>> history(const int limit = 100, const std::string& before = "", const std::string& after = "", const std::string& around = "");

    ///Delete multiple messages from this channel
    DLL_EXPORT void delete_messages(const std::vector<std::shared_ptr<Message>>& messages);
};

}  // namespace DiscordCPP
