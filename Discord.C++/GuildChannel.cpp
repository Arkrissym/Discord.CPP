#include "static.h"
#include "GuildChannel.h"
#include "Guild.h"

#include <cpprest/http_client.h>

using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace utility;
using namespace std;

DiscordCPP::GuildChannel::GuildChannel(const value& data, const string_t& token) : DiscordCPP::TextChannel(data, token) {
	if (is_valid_field("guild_id"))
		guild = new Guild(NULL, conversions::to_utf8string(data.at(U("guild_id")).as_string()), token);

	if (is_valid_field("parent_id"))
		parent = new Channel(conversions::to_utf8string(data.at(U("parent_id")).as_string()), token);
}

DiscordCPP::GuildChannel::GuildChannel(const string& id, const string_t& token) {
	string url = "/channels/" + id;
	_token = token;
	*this = GuildChannel(api_call(url), token);
}

DiscordCPP::GuildChannel::GuildChannel(const GuildChannel& old) : DiscordCPP::TextChannel(old) {
	if (old.guild != NULL)
		guild = new Guild(*old.guild);
	topic = old.topic;
	if (old.parent != NULL)
		parent = new Channel(*old.parent);
}

DiscordCPP::GuildChannel::GuildChannel() {

}

DiscordCPP::GuildChannel::~GuildChannel() {
	if (guild != NULL)
		delete guild;
	if (parent != NULL)
		delete parent;
}
