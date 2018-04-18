#include "static.h"
#include "VoiceChannel.h"

#include <cpprest\http_client.h>

using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace utility;
using namespace std;

DiscordCPP::VoiceChannel::VoiceChannel(value data, string_t token) : DiscordCPP::GuildChannel(data, token) {
	//_log = Logger("discord.voicechannel");

	if (is_valid_field("bitrate"))
		bitrate = data.at(U("bitrate")).as_integer();

	if (is_valid_field("user_limit"))
		user_limit = data.at(U("user_limit")).as_integer();
}

DiscordCPP::VoiceChannel::VoiceChannel(string id, string_t token) {
	//_log = Logger("discord.voicechannel");

	string url = "/channels/" + id;

	http_client c(U(API_URL));
	http_request request(methods::GET);

	request.set_request_uri(uri(conversions::to_string_t(url)));
	request.headers().add(U("Authorization"), conversions::to_string_t("Bot " + conversions::to_utf8string(token)));

	Concurrency::task<void> requestTask = c.request(request).then([this, token](http_response response) {
		string response_string = response.extract_utf8string().get();

		//_log.debug(response_string);

		value data = value::parse(conversions::to_string_t(response_string));

		*this = VoiceChannel(data, token);
	});

	try {
		requestTask.wait();
	}
	catch (const std::exception &e) {
		Logger("discord.voicechannel").error("Error exception: " + string(e.what()));
	}
}

DiscordCPP::VoiceChannel::VoiceChannel(const VoiceChannel & old) : DiscordCPP::GuildChannel(old) {
	bitrate = old.bitrate;
	user_limit = old.user_limit;
}

DiscordCPP::VoiceChannel::VoiceChannel() {
	//_log = Logger("discord.voicechannel");
}

DiscordCPP::VoiceChannel::~VoiceChannel() {

}