#include "Message.h"
#include "static.h"
#include "VoiceChannel.h"
#include "GuildChannel.h"
#include "DMChannel.h"
#include "User.h"
#include "Embed.h"

#include "Logger.h"

#include <cpprest\http_client.h>

using namespace web::http;
using namespace web::http::client;
using namespace std;
using namespace web::json;
using namespace utility;

/**	@param[in]	data	raw JSON data
	@param[in]	token	discord token for the API
	@return	Message object
*/
DiscordCPP::Message::Message(value data, string_t token) : DiscordCPP::DiscordObject(token) {
	//_log = Logger("discord.message");

	if(is_valid_field("id"))
		id = conversions::to_utf8string(data.at(U("id")).as_string());
	
	if (is_valid_field("channel_id")) {
		string url = "/channels/" + conversions::to_utf8string(data.at(U("channel_id")).as_string());

		value channel_data = api_call(url);
		//Logger("discord.message").debug(conversions::to_utf8string(channel_data.serialize()));
		switch (channel_data.at(U("type")).as_integer()) {
		case ChannelType::GUILD_TEXT:
			channel = (TextChannel *)new GuildChannel(channel_data, token);
			break;
		case ChannelType::DM:
		case ChannelType::GROUP_DM:
			channel = (TextChannel *)new DMChannel(channel_data, token);
			break;
		default:
			channel = new TextChannel(channel_data, token);
		}
	}

	if (is_valid_field("author"))
		author = new User(data.at(U("author")), token);

	if (is_valid_field("content"))
		content = conversions::to_utf8string(data.at(U("content")).as_string());

	if (is_valid_field("timestamp"))
		timestamp = conversions::to_utf8string(data.at(U("timestamp")).as_string());

	if (is_valid_field("edited_timestamp"))
		edited_timestamp = conversions::to_utf8string(data.at(U("edited_timestamp")).as_string());

	if (is_valid_field("tts"))
		tts = data.at(U("tts")).as_bool();

	if (is_valid_field("mention_everyone"))
		tts = data.at(U("mention_everyone")).as_bool();

	if (is_valid_field("mentions")) {
		web::json::array tmp = data.at(U("mentions")).as_array();
		for (int i = 0; i < tmp.size(); i++)
			mentions.push_back(new User(tmp[i], token));
	}

	//mention_roles

	//attachements

	if (is_valid_field("embeds")) {
		web::json::array tmp = data.at(U("embeds")).as_array();
		for (int i = 0; i < tmp.size(); i++) {
			embeds.push_back(new Embed(tmp[i]));
		}
	}

	//reactions

	if (is_valid_field("pinned"))
		pinned = data.at(U("pinned")).as_bool();

	if (is_valid_field("webhook_id"))
		webhook_id = conversions::to_utf8string(data.at(U("webhook_id")).as_string());

	if (is_valid_field("type"))
		type = data.at(U("type")).as_integer();

	//activity

	//application

	//_log.debug("created message object");
}

/**	@param[in]	old	Message object to copy
	@return		copied Message object 
*/
DiscordCPP::Message::Message(const Message & old) {
	//_log = old._log;
	_token = old._token;
	id = old.id;
	
	if (old.channel != NULL) {
		try {
			channel = (TextChannel *)old.channel->copy(*old.channel);
		}
		catch (std::exception &e) {
			Logger("discord.message").error("Error in channel copy: " + string(e.what()));
		}
	}
	else {
		channel = NULL;
	}

	if (old.author != NULL)
		author = new User(*old.author);
	content = old.content;
	timestamp = old.timestamp;
	edited_timestamp = old.edited_timestamp;
	tts = old.tts;
	mention_everyone = old.mention_everyone;
	//mentions = old.mentions;
	for (int i = 0; i < old.mentions.size(); i++) {
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
	//_log = Logger("discord.message");
	//_log.debug("created empty message object");
}

DiscordCPP::Message::~Message() {
	//_log.debug("destroyed message object");

	//if(channel != NULL)
	delete channel;
	delete author;
	for (int i = 0; i < mentions.size(); i++) {
		delete mentions[i];
	}
	for (int i = 0; i < embeds.size(); i++) {
		delete embeds[i];
	}
}

/**	@param[in]	content	New message-content.
	@return		Updated message object.
*/
DiscordCPP::Message DiscordCPP::Message::edit(string content) {
	string url = "/channels/" + channel->id + "/messages/" + id;

	value data;
	data[U("content")] = value(conversions::to_string_t(content));

	return Message(api_call(url, methods::PATCH, data), _token);
}

void DiscordCPP::Message::delete_msg() {
	string url = "/channels/" + channel->id + "/messages/" + id;
	api_call(url, methods::DEL);
}
