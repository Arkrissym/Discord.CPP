#include "Message.h"

#include <optional>
#include <string>

#include "DMChannel.h"
#include "DiscordObject.h"
#include "Embed.h"
#include "Guild.h"
#include "GuildChannel.h"
#include "Logger.h"
#include "TextChannel.h"
#include "User.h"
#include "VoiceChannel.h"
#include "static.h"

/**	@param[in]	data	raw JSON data
    @param[in]	token	discord token for the API
    @return	Message object
*/
DiscordCPP::Message::Message(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token, data.at("id").get<std::string>()),
      author{data.at("author"), token} {
    data.at("channel_id").get_to<std::string>(channel_id);
    guild_id = get_optional<std::string>(data, "guild_id");

    data.at("content").get_to<std::string>(content);
    data.at("timestamp").get_to<std::string>(timestamp);
    edited_timestamp = get_or_else<std::string>(data, "edited_timestamp", "");
    tts = get_or_else(data, "tts", false);
    mention_everyone = get_or_else(data, "mention_everyone", false);

    if (has_value(data, "mentions")) {
        for (json mention : data.at("mentions")) {
            mentions.emplace_back(mention, token);
        }
    }

    // mention_roles

    // attachements

    if (has_value(data, "embeds")) {
        for (json embed : data.at("embeds")) {
            embeds.emplace_back(embed);
        }
    }

    // reactions

    pinned = get_or_else(data, "pinned", false);
    webhook_id = get_or_else<std::string>(data, "webhook_id", "");
    type = static_cast<Type>(data.at("type").get<int>());

    // activity

    // application
}

/**	@param[in]	old	Message object to copy
    @return		copied Message object
*/
DiscordCPP::Message::Message(const Message& old)
    : DiscordCPP::DiscordObject(old),
      channel_id(old.channel_id),
      guild_id(old.guild_id),
      author(old.author),
      content(old.content),
      timestamp(old.timestamp),
      edited_timestamp(old.edited_timestamp),
      tts(old.tts),
      mention_everyone(old.mention_everyone),
      mentions(old.mentions),
      embeds(old.embeds),
      pinned(old.pinned),
      webhook_id(old.webhook_id),
      type(old.type) {
    // mention_roles
    // attachements
    // reactions
    // activity
    // application

    if (old.channel != nullptr) {
        channel = new TextChannel(*old.channel);
    }
    if (guild != nullptr) {
        guild = new Guild(*old.guild);
    }
}

DiscordCPP::Message::~Message() {
    if (channel != nullptr) {
        delete channel;
    }
    if (guild != nullptr) {
        delete guild;
    }
}

/**	@param[in]	content	New message-content.
    @return		Updated message object.
*/
DiscordCPP::Message DiscordCPP::Message::edit(const std::string& content) {
    std::string url = "/channels/" + channel_id + "/messages/" + get_id();

    json data = {{"content", content}};

    return {api_call(url, "PATCH", data, "application/json"), get_token()};
}

void DiscordCPP::Message::delete_msg() {
    std::string url = "/channels/" + channel_id + "/messages/" + get_id();
    api_call(url, "DELETE");
}

/**	@param[in]	content	The string message to send.
    @param[in]	tts		(optional) Wether to send as tts-message or not. Default is false.
    @return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::Message::reply(const std::string& content, const bool tts) {
    std::string url = "/channels/" + channel_id + "/messages";

    json data = {{"content", content}, {"tts", tts}, {"message_reference", {{"message_id", get_id()}}}};

    return {api_call(url, "POST", data, "application/json"), get_token()};
}

/**	@param[in]	embed	The Embed to send.
    @return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::Message::reply(DiscordCPP::Embed embed) {
    std::string url = "/channels/" + channel_id + "/messages";

    json data;
    data["message_reference"] = {{"message_id", get_id()}};
    data["embeds"].push_back(embed.to_json());

    return {api_call(url, "POST", data, "application/json"), get_token()};
}

DiscordCPP::TextChannel DiscordCPP::Message::get_channel() {
    if (channel == nullptr) {
        channel = new TextChannel(channel_id, get_token());
    }
    return *channel;
}

std::optional<DiscordCPP::Guild> DiscordCPP::Message::get_guild() {
    if (guild_id.has_value() && guild == nullptr) {
        guild = new Guild(nullptr, guild_id.value(), get_token());
    }
    return (guild != nullptr) ? std::optional<Guild>{*guild} : std::nullopt;
}
