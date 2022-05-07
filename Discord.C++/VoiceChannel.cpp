#include "VoiceChannel.h"

#include "Discord.h"
#include "Exceptions.h"
#include "Guild.h"
#include "Logger.h"
#include "VoiceClient.h"

DiscordCPP::VoiceChannel::VoiceChannel(Discord* client, const json& data, const std::string& token) : DiscordCPP::Channel(data, token) {
    _client = client;
    bitrate = get_or_else<int>(data, "bitrate", 0);
    user_limit = get_or_else<int>(data, "user_limit", 0);

    if (has_value(data, "parent_id")) {
        parent = new Channel(data.at("parent_id").get<std::string>(), token);
    }

    if (has_value(data, "guild_id")) {
        guild = new Guild(client, data.at("guild_id").get<std::string>(), token);
    }
}

DiscordCPP::VoiceChannel::VoiceChannel(Discord* client, const std::string& id, const std::string& token) {
    _token = token;
    std::string url = "/channels/" + id;
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
DiscordCPP::VoiceClient DiscordCPP::VoiceChannel::connect() {
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

    json payload = {
        {"op", 4},  //
        {"d", {
                  {"guild_id", guild->id},   //
                  {"channel_id", this->id},  //
                  {"self_mute", false},      //
                  {"self_deaf", false}       //
              }}                             //
    };

    unsigned int shard_id = (stoull(guild->id) >> 22) % _client->_num_shards;
    std::shared_ptr<MainGateway> current_gateway = _client->get_shard(shard_id);

    current_gateway->send(payload).get();
    Logger("discord.voicechannel").debug("Payload with Opcode 4 (Gateway Voice State Update) has been sent");

    while (true) {
        for (auto voice_state : _client->_voice_states) {
            if ((voice_state->channel_id == this->id) && (voice_state->endpoint.length() > 1)) {
                Logger("discord.voicechannel").debug("Creating new voice client.");

                return VoiceClient(current_gateway, voice_state->voice_token, voice_state->endpoint, voice_state->session_id, voice_state->guild_id, voice_state->channel_id, _client->_user->id);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return VoiceClient();
}
