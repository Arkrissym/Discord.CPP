#include "static.h"
#include "GuildChannel.h"

#include <cpprest\http_client.h>

using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace utility;
using namespace std;

DiscordCPP::GuildChannel::GuildChannel(value data, string_t token) : DiscordCPP::Channel(data, token) {
	//_log = Logger("discord.guildchannel");

	if (is_valid_field("guild_id"))
		guild = new Guild(conversions::to_utf8string(data.at(U("guild_id")).as_string()), token);
	
	if (is_valid_field("topic"))
		topic = conversions::to_utf8string(data.at(U("topic")).as_string());

	//parent
}

DiscordCPP::GuildChannel::GuildChannel(string id, string_t token) {
	//_log = Logger("discord.guildchannel");

	string url = "/channels/" + id;

	*this = GuildChannel(api_call(url), token);
}

DiscordCPP::GuildChannel::GuildChannel(const GuildChannel & old) : DiscordCPP::Channel(old) {
	if (old.guild != NULL)
		guild = new Guild(*old.guild);
	topic = old.topic;
	parent_id = old.parent_id;
}

DiscordCPP::GuildChannel::GuildChannel() {
	//_log = Logger("discord.guildchannel");
}

DiscordCPP::GuildChannel::~GuildChannel() {
	delete guild;
}