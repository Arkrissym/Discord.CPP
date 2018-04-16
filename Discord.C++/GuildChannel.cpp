#include "static.h"
#include "GuildChannel.h"

#include <cpprest\http_client.h>

using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace utility;
using namespace std;

DiscordCPP::GuildChannel::GuildChannel(value data, string_t token) : DiscordCPP::Channel(data, token) {
	if (is_valid_field("guild_id"))
		guild = Guild(conversions::to_utf8string(data.at(U("guild_id")).as_string()), token);
	
	if (is_valid_field("topic"))
		topic = conversions::to_utf8string(data.at(U("topic")).as_string());
}

DiscordCPP::GuildChannel::GuildChannel(string id, string_t token) {
	_log = Logger("discord.guildchannel");

	string url = "/channels/" + id;

	http_client c(U(API_URL));
	http_request request(methods::GET);

	request.set_request_uri(uri(conversions::to_string_t(url)));
	request.headers().add(U("Authorization"), conversions::to_string_t("Bot " + conversions::to_utf8string(token)));

	Concurrency::task<void> requestTask = c.request(request).then([this, token](http_response response) {
		string response_string = response.extract_utf8string().get();

		//_log.debug(response_string);

		value data = value::parse(conversions::to_string_t(response_string));

		*this = GuildChannel(data, token);
	});

	try {
		requestTask.wait();
	}
	catch (const std::exception &e) {
		_log.error("Error exception: " + string(e.what()));
	}
}

DiscordCPP::GuildChannel::GuildChannel() {
	_log = Logger("discord.guildchannel");
}

DiscordCPP::GuildChannel::~GuildChannel() {

}
