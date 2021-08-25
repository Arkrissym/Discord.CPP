#include "VoiceChannel.h"

#include "Discord.h"
#include "Exceptions.h"
#include "Guild.h"
#include "Logger.h"
#include "VoiceClient.h"
#include "static.h"

using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace utility;
using namespace std;

DiscordCPP::VoiceChannel::VoiceChannel(Discord* client, const value& data, const string_t& token) : DiscordCPP::Channel(data, token) {
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

DiscordCPP::VoiceChannel::VoiceChannel(Discord* client, const string& id, const string_t& token) {
    string url = "/channels/" + id;
    _token = token;
    *this = VoiceChannel(client, api_call(url), token);
}

DiscordCPP::VoiceChannel::VoiceChannel(const VoiceChannel& old) : DiscordCPP::Channel(old) {
    bitrate = old.bitrate;
    user_limit = old.user_limit;
    if (old.parent != NULL)
        parent = new Channel(*old.parent);
}

DiscordCPP::VoiceChannel::~VoiceChannel() {
    delete parent;
    delete guild;
}

/*	@return		VoiceClient
	@throws	ClientException
**/
DiscordCPP::VoiceClient* DiscordCPP::VoiceChannel::connect() {
    if (this->type != ChannelType::GUILD_VOICE) {
        throw ClientException("channel must be a voice channel");
    }

    const Guild* guild = NULL;
    for (unsigned int i = 0; i < _client->_guilds.size(); i++) {
        for (unsigned int j = 0; j < _client->_guilds[i]->channels.size(); j++) {
            if (_client->_guilds[i]->channels[j]->id == this->id)
                guild = _client->_guilds[i];
        }
    }

    unsigned int shard_id = (stoull(guild->id) >> 22) % _client->_num_shards;
    unsigned int gw_id = 0;
    for (unsigned int i = 0; i < _client->_gateways.size(); i++) {
        if (_client->_gateways[i]->get_shard_id() == shard_id) {
            gw_id = i;
            break;
        }
    }

    value payload = value();
    payload[U("op")] = value(4);
    payload[U("d")][U("guild_id")] = value(conversions::to_string_t(guild->id));
    payload[U("d")][U("channel_id")] = value(conversions::to_string_t(this->id));
    payload[U("d")][U("self_mute")] = value(false);
    payload[U("d")][U("self_deaf")] = value(false);

    auto current_gateway = _client->_gateways[gw_id];
    current_gateway->send(payload).wait();
    Logger("discord.voicechannel").debug("Payload with Opcode 4 (Gateway Voice State Update) has been sent");

    while (true) {
        for (unsigned int i = 0; i < _client->_voice_states.size(); i++) {
            if ((_client->_voice_states[i]->channel_id == conversions::to_string_t(this->id)) && (_client->_voice_states[i]->endpoint.length() > 1)) {
                return new VoiceClient(&current_gateway, _client->_voice_states[i]->voice_token, _client->_voice_states[i]->endpoint, _client->_voice_states[i]->session_id, _client->_voice_states[i]->guild_id, _client->_voice_states[i]->channel_id, conversions::to_string_t(_client->_user->id));
            }
        }

        waitFor(chrono::milliseconds(10)).wait();
    }

    return new VoiceClient();
}
