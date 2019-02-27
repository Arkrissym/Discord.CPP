#include "static.h"
#include "GuildChannel.h"
#include "Guild.h"

#include <cpprest/http_client.h>

using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace utility;
using namespace std;

DiscordCPP::GuildChannel::GuildChannel(value data, string_t token) : DiscordCPP::TextChannel(data, token) {
	//_log = Logger("discord.guildchannel");

	if (is_valid_field("guild_id"))	//TODO: remove NULL-pointer 
		guild = new Guild(NULL, conversions::to_utf8string(data.at(U("guild_id")).as_string()), token);

	if (is_valid_field("parent_id"))
		parent = new Channel(conversions::to_utf8string(data.at(U("parent_id")).as_string()), token);
}

DiscordCPP::GuildChannel::GuildChannel(string id, string_t token) {
	//_log = Logger("discord.guildchannel");

	string url = "/channels/" + id;
	_token = token;
	*this = GuildChannel(api_call(url), token);
}

DiscordCPP::GuildChannel::GuildChannel(const GuildChannel & old) : DiscordCPP::TextChannel(old) {
	if (old.guild != NULL)
		guild = new Guild(*old.guild);
	topic = old.topic;
	if (old.parent != NULL)
		parent = new Channel(*old.parent);
}

DiscordCPP::GuildChannel::GuildChannel() {
	//_log = Logger("discord.guildchannel");
}

DiscordCPP::GuildChannel::~GuildChannel() {
	delete guild;
	delete parent;
}
