#include "TextChannel.h"

#include "Channel.h"
#include "DiscordObject.h"
#include "Embed.h"
#include "Exceptions.h"
#include "Message.h"

DiscordCPP::TextChannel::TextChannel(const json& data, const std::string& token)
    : DiscordCPP::Channel(data, token) {
    topic = get_or_else<std::string>(data, "topic", "");
    nsfw = get_or_else<bool>(data, "nsfw", false);
    last_message_id = get_or_else<std::string>(data, "last_message_id", "");
    last_pin_timestamp = get_or_else<std::string>(data, "last_pin_timestamp", "");
    rate_limit_per_user = get_or_else<int>(data, "rate_limit_per_user", 0);
}

DiscordCPP::TextChannel::TextChannel(const std::string& id, const std::string& token)
    : DiscordCPP::Channel(token) {
    std::string url = "/channels/" + id;
    *this = TextChannel(api_call(url), token);
}

DiscordCPP::TextChannel::TextChannel(const std::string& token)
    : DiscordCPP::Channel(token) {
}

/**	@param[in]	content	The string message to send.
    @param[in]	tts		(optional) Wether to send as tts-message or not. Default is false.
    @return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::TextChannel::send(const std::string& content, const bool tts) {
    std::string url = "/channels/" + get_id() + "/messages";

    json data = {{"content", content}, {"tts", tts}};

    return {api_call(url, "POST", data, "application/json"), get_token()};
}

/**	@param[in]	embed	The Embed to send.
    @return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::TextChannel::send(DiscordCPP::Embed embed) {
    std::string url = "/channels/" + get_id() + "/messages";

    json data;
    data["embeds"].push_back(embed.to_json());

    return {api_call(url, "POST", data, "application/json"), get_token()};
}

/**	@param[in]	limit	Max number of messages to retrieve (1-100)
    @param[in]	before	Get messages before this message id
    @param[in]	after	Get messages after this message id
    @param[in]	around	Get messages around this message id
    @return	Array of messages
*/
std::vector<std::shared_ptr<DiscordCPP::Message>> DiscordCPP::TextChannel::history(const int limit, const std::string& before, const std::string& after, const std::string& around) {
    std::vector<std::shared_ptr<Message>> ret;

    std::string url = "/channels/" + get_id() + "/messages" + "?limit=" + std::to_string(limit);

    if (before.length() > 0)
        url += "&before=" + before;
    if (after.length() > 0)
        url += "&after=" + after;
    if (around.length() > 0)
        url += "&around" + around;

    json msgs = api_call(url, "GET", json(), "", false);

    for (const json& msg : msgs) {
        ret.push_back(std::make_shared<Message>(Message(msg, get_id())));
    }

    return ret;
}

/**	@param[in]	messages	vector containing the messages to delete
    @throws	SizeError
*/
void DiscordCPP::TextChannel::delete_messages(const std::vector<std::shared_ptr<Message>>& messages) {
    if (messages.size() < 2) {
        throw SizeError("Cannot delete less than 2 messages: use Message::delete_msg() instead");
    } else if (messages.size() > 100) {
        throw SizeError("Cannot delete more than 100 messages");
    }

    std::string url = "/channels/" + get_id() + "/messages/bulk-delete";

    json data = {{"messages", json::array()}};
    for (const auto& message : messages) {
        data["messages"].emplace_back(message->get_id());
    }

    api_call(url, "POST", data, "application/json");
}
