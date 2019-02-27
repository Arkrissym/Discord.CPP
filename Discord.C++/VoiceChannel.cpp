#include "static.h"
#include "VoiceChannel.h"
#include "Guild.h"
#include "VoiceClient.h"
#include "Discord.h"
#include "Logger.h"

#include <cpprest/http_client.h>

using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace utility;
using namespace std;

DiscordCPP::VoiceChannel::VoiceChannel(Discord *client, value data, string_t token) : DiscordCPP::Channel(data, token) {
	//_log = Logger("discord.voicechannel");

	_client = client;

	if (is_valid_field("bitrate"))
		bitrate = data.at(U("bitrate")).as_integer();

	if (is_valid_field("user_limit"))
		user_limit = data.at(U("user_limit")).as_integer();

	if (is_valid_field("parent_id"))
		parent = new Channel(conversions::to_utf8string(data.at(U("parent_id")).as_string()), token);

	if (is_valid_field("guild_id"))
		guild = new Guild(client, conversions::to_utf8string(data.at(U("guild_id")).as_string()), token);
}

DiscordCPP::VoiceChannel::VoiceChannel(Discord *client, string id, string_t token) {
	//_log = Logger("discord.voicechannel");

	string url = "/channels/" + id;
	_token = token;
	*this = VoiceChannel(client, api_call(url), token);
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
	delete guild;
}

/// @return		VoiceClient
DiscordCPP::VoiceClient * DiscordCPP::VoiceChannel::connect() {
	if (this->type != ChannelType::GUILD_VOICE) {
		throw logic_error("channel must be a voice channel");
	}

	Guild *guild = NULL;
	for (unsigned int i = 0; i < _client->_guilds.size(); i++) {
		for (unsigned int j = 0; j < _client->_guilds[i]->channels.size(); j++) {
			if (_client->_guilds[i]->channels[j]->id == this->id)
				guild = _client->_guilds[i];
		}
	}

	value payload = value();
	payload[U("op")] = value(4);
	payload[U("d")][U("guild_id")] = value(conversions::to_string_t(guild->id));
	payload[U("d")][U("channel_id")] = value(conversions::to_string_t(this->id));
	payload[U("d")][U("self_mute")] = value(false);
	payload[U("d")][U("self_deaf")] = value(false);

	websocket_outgoing_message msg;
	msg.set_utf8_message(conversions::to_utf8string(payload.serialize()));

	_client->_client->send(msg).then([this] {
		Logger("discord.voicechannel").debug("Payload with Opcode 4 (Gateway Voice State Update) has been sent");
	});

	while (true) {
		for (unsigned int i = 0; i < _client->_voice_states.size(); i++) {
			if ((_client->_voice_states[i]->channel_id == conversions::to_string_t(this->id)) && (_client->_voice_states[i]->endpoint.length() > 1)) {
				return new VoiceClient(&_client->_client, _client->_voice_states[i]->voice_token, _client->_voice_states[i]->endpoint, _client->_voice_states[i]->session_id, _client->_voice_states[i]->guild_id, _client->_voice_states[i]->channel_id, conversions::to_string_t(_client->_user->id));
			}
		}
		//this_thread::sleep_for(chrono::milliseconds(10));
		waitFor(chrono::milliseconds(10)).wait();
	}

	return new VoiceClient();
}
