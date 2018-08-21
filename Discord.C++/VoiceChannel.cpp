#include "static.h"
#include "VoiceChannel.h"

#include <cpprest/http_client.h>

using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace utility;
using namespace std;

DiscordCPP::VoiceChannel::VoiceChannel(value data, string_t token) : DiscordCPP::Channel(data, token) {
	//_log = Logger("discord.voicechannel");

	if (is_valid_field("bitrate"))
		bitrate = data.at(U("bitrate")).as_integer();

	if (is_valid_field("user_limit"))
		user_limit = data.at(U("user_limit")).as_integer();

	if (is_valid_field("parent_id"))
		parent = new Channel(conversions::to_utf8string(data.at(U("parent_id")).as_string()), token);
}

DiscordCPP::VoiceChannel::VoiceChannel(string id, string_t token) {
	//_log = Logger("discord.voicechannel");

	string url = "/channels/" + id;
	_token = token;
	*this = VoiceChannel(api_call(url), token);
}

DiscordCPP::VoiceChannel::VoiceChannel(const VoiceChannel & old) : DiscordCPP::Channel(old) {
	bitrate = old.bitrate;
	user_limit = old.user_limit;
	if (old.parent != NULL)
		parent=new Channel(*old.parent);
}

DiscordCPP::VoiceChannel::VoiceChannel() {
	//_log = Logger("discord.voicechannel");
}

DiscordCPP::VoiceChannel::~VoiceChannel() {
	delete parent;
}
