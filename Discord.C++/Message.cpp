#include "Message.h"

#include <cpprest/http_client.h>

#include "DMChannel.h"
#include "Embed.h"
#include "GuildChannel.h"
#include "Logger.h"
#include "User.h"
#include "VoiceChannel.h"

/**	@param[in]	data	raw JSON data
	@param[in]	token	discord token for the API
	@return	Message object
*/
DiscordCPP::Message::Message(const json& data, const std::string& token) : DiscordCPP::DiscordObject(token) {
    data.at("id").get_to<std::string>(id);

    std::string url = "/channels/" + data.at("channel_id").get<std::string>();
    json channel_data = api_call(url);
    channel = (TextChannel*)Channel::from_json(NULL, channel_data, token);

    if (has_value(data, "author")) {
        author = new User(data.at("author"), token);
    }

    data.at("content").get_to<std::string>(content);
    data.at("timestamp").get_to<std::string>(timestamp);
    edited_timestamp = get_or_else<std::string>(data, "edited_timestamp", "");
    tts = get_or_else(data, "tts", false);
    mention_everyone = get_or_else(data, "mention_everyone", false);

    if (has_value(data, "mentions")) {
        for (json mention : data.at("mentions")) {
            mentions.push_back(new User(mention, token));
        }
    }

    //mention_roles

    //attachements

    if (has_value(data, "embeds")) {
        for (json embed : data.at("embeds")) {
            embeds.push_back(new Embed(embed));
        }
    }

    //reactions

    pinned = get_or_else(data, "pinned", false);
    webhook_id = get_or_else<std::string>(data, "webhook_id", "");
    data.at("type").get_to<int>(type);

    //activity

    //application
}

/**	@param[in]	old	Message object to copy
	@return		copied Message object
*/
DiscordCPP::Message::Message(const Message& old) {
    _token = old._token;
    id = old.id;

    if (old.channel != NULL) {
        try {
            channel = (TextChannel*)old.channel->copy(*old.channel);
        } catch (std::exception& e) {
            Logger("discord.message").error("Error in channel copy: " + std::string(e.what()));
        }
    } else {
        channel = NULL;
    }

    if (old.author != NULL)
        author = new User(*old.author);
    content = old.content;
    timestamp = old.timestamp;
    edited_timestamp = old.edited_timestamp;
    tts = old.tts;
    mention_everyone = old.mention_everyone;
    for (unsigned int i = 0; i < old.mentions.size(); i++) {
        mentions.push_back(new User(*old.mentions[i]));
    }
    //mention_roles
    //attachements
    //embeds
    //reactions
    pinned = old.pinned;
    webhook_id = old.webhook_id;
    type = old.type;
    //activity
    //application
}

DiscordCPP::Message::Message() {
}

DiscordCPP::Message::~Message() {
    if (channel != NULL)
        delete channel;
    if (author != NULL)
        delete author;
    for (unsigned int i = 0; i < mentions.size(); i++) {
        delete mentions[i];
    }
    for (unsigned int i = 0; i < embeds.size(); i++) {
        delete embeds[i];
    }
}

/**	@param[in]	content	New message-content.
	@return		Updated message object.
*/
DiscordCPP::Message DiscordCPP::Message::edit(const std::string& content) {
    std::string url = "/channels/" + channel->id + "/messages/" + id;

    json data = {{"content", content}};

    return Message(api_call(url, web::http::methods::PATCH, data), _token);
}

void DiscordCPP::Message::delete_msg() {
    std::string url = "/channels/" + channel->id + "/messages/" + id;
    api_call(url, web::http::methods::DEL);
}
