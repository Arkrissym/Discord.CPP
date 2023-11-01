#include "Message.h"

#include "DMChannel.h"
#include "DiscordObject.h"
#include "Embed.h"
#include "Guild.h"
#include "GuildChannel.h"
#include "Logger.h"
#include "User.h"
#include "VoiceChannel.h"

/**	@param[in]	data	raw JSON data
    @param[in]	token	discord token for the API
    @return	Message object
*/
DiscordCPP::Message::Message(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token, data.at("id").get<std::string>()) {
    data.at("channel_id").get_to<std::string>(channel_id);
    author = User(data.at("author"), token);

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
            embeds.push_back(Embed(embed));
        }
    }

    // reactions

    pinned = get_or_else(data, "pinned", false);
    webhook_id = get_or_else<std::string>(data, "webhook_id", "");
    data.at("type").get_to<int>(type);

    // activity

    // application
}

/**	@param[in]	old	Message object to copy
    @return		copied Message object
*/
DiscordCPP::Message::Message(const Message& old)
    : DiscordCPP::DiscordObject(old) {
    channel_id = old.channel_id;
    if (old.channel != nullptr) {
        channel = (TextChannel*)old.channel->copy();
    }
    guild_id = old.guild_id;
    if (guild != nullptr) {
        guild = new Guild(*old.guild);
    }

    author = old.author;
    content = old.content;
    timestamp = old.timestamp;
    edited_timestamp = old.edited_timestamp;
    tts = old.tts;
    mention_everyone = old.mention_everyone;
    mentions = old.mentions;
    // mention_roles
    // attachements
    embeds = old.embeds;
    // reactions
    pinned = old.pinned;
    webhook_id = old.webhook_id;
    type = old.type;
    // activity
    // application
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
    std::string url = "/channels/" + channel->get_id() + "/messages/" + get_id();

    json data = {{"content", content}};

    return Message(api_call(url, "PATCH", data, "application/json"), get_token());
}

void DiscordCPP::Message::delete_msg() {
    std::string url = "/channels/" + channel->get_id() + "/messages/" + get_id();
    api_call(url, "DELETE");
}

/**	@param[in]	content	The string message to send.
    @param[in]	tts		(optional) Wether to send as tts-message or not. Default is false.
    @return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::Message::reply(const std::string& content, const bool tts) {
    std::string url = "/channels/" + channel_id + "/messages";

    json data = {{"content", content}, {"tts", tts}, {"message_reference", {"message_id", get_id()}}};

    return Message(api_call(url, "POST", data, "application/json"), get_token());
}

/**	@param[in]	embed	The Embed to send.
    @return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::Message::reply(DiscordCPP::Embed embed) {
    std::string url = "/channels/" + channel_id + "/messages";

    json data;
    data["message_reference"]["message_id"] = get_id();
    data["embeds"].push_back(embed.to_json());

    return Message(api_call(url, "POST", data, "application/json"), get_token());
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
    return (guild != nullptr) ? *guild : std::make_optional<DiscordCPP::Guild>();
}
