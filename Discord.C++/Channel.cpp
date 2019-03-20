#include "Channel.h"
#include "static.h"
#include "Discord.h"

#include <cpprest/http_client.h>

using namespace std;
using namespace web::json;
using namespace utility;
using namespace web::http;
using namespace web::http::client;

/**	@param[in]	data	JSON data
	@param[in]	token	discord token
*/
DiscordCPP::Channel::Channel(value data, string_t token) : DiscordCPP::DiscordObject(token) {
	//_log = Logger("discord.channel");

	if (is_valid_field("id"))
		id = conversions::to_utf8string(data.at(U("id")).as_string());

	if (is_valid_field("type"))
		type = data.at(U("type")).as_integer();

	if (is_valid_field("position"))
		position = data.at(U("position")).as_integer();

	//permission_overwrites

	if (is_valid_field("name"))
		name = conversions::to_utf8string(data.at(U("name")).as_string());

	if (is_valid_field("icon"))
		icon = conversions::to_utf8string(data.at(U("icon")).as_string());

	//_log.debug("created channel object");
}

/**	@param[in]	id		the channel's id
	@param[in]	token	discord token
*/
DiscordCPP::Channel::Channel(string id, string_t token) : DiscordCPP::DiscordObject(token) {
	//_log = Logger("discord.channel");

	string url = "/channels/" + id;

	*this = Channel(api_call(url), token);
}

/*	@param[in]	old	the Channel to copy
*/
DiscordCPP::Channel::Channel(const Channel & old) {
	//_log = old._log;
	_token = old._token;
	id = old.id;
	type = old.type;
	position = old.position;
	//permission_overwrites
	name = old.name;
	nsfw = old.nsfw;
	last_message_id = old.last_message_id;
	icon = old.icon;
	last_pin_timestamp = old.last_pin_timestamp;
}

DiscordCPP::Channel::Channel() {
	//_log = Logger("discord.channel");
	//_log.debug("created empty channel object");
}

DiscordCPP::Channel * DiscordCPP::Channel::from_json(Discord * client, value data, string_t token) {
	Channel *channel;

	switch (data.at(U("type")).as_integer()) {
	case ChannelType::GUILD_TEXT:
	case ChannelType::GUILD_NEWS:
		channel = (Channel *)new TextChannel(data, token);
		break;
	case ChannelType::GUILD_VOICE:
		channel = (Channel *)new VoiceChannel(client, data, token);
		break;
	case ChannelType::DM:
	case ChannelType::GROUP_DM:
		channel = new DMChannel(data, token);
		break;
	default:
		channel = new Channel(data, token);
	}

	return channel;
}

void DiscordCPP::Channel::delete_channel() {
	string url = "/channels/" + id;

	api_call(url, methods::DEL);
}
