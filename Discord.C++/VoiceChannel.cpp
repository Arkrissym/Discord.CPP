#include "VoiceChannel.h"

#include <cstddef>

#include "Discord.h"
#include "Exceptions.h"
#include "Guild.h"
#include "Logger.h"
#include "VoiceClient.h"

DiscordCPP::VoiceChannel::VoiceChannel(Discord* client, const json& data, const std::string& token)
    : DiscordCPP::Channel(data, token), _client(client) {
    bitrate = get_or_else<int>(data, "bitrate", 0);
    user_limit = get_or_else<int>(data, "user_limit", 0);
    parent_id = get_optional<std::string>(data, "parent_id");
}

DiscordCPP::VoiceChannel::VoiceChannel(Discord* client, const std::string& id, const std::string& token) {
    std::string url = "/channels/" + id;
    *this = VoiceChannel(client, api_call(url), token);
}

/*	@return		VoiceClient
    @throws     ClientException
**/
std::shared_ptr<DiscordCPP::VoiceClient> DiscordCPP::VoiceChannel::connect() {
    if (this->get_type() != ChannelType::GUILD_VOICE) {
        throw ClientException("channel must be a voice channel");
    }

    const Guild* guild = nullptr;
    for (auto& _guild : _client->_guilds) {
        auto channels = _guild->get_channels();
        for (auto& channel : channels) {
            if (channel->get_id() == get_id())
                guild = _guild;
        }
    }

    json payload = {
        {"op", 4},  //
        {"d", {
                  {"guild_id", guild->get_id()},  //
                  {"channel_id", get_id()},       //
                  {"self_mute", false},           //
                  {"self_deaf", false}            //
              }}                                  //
    };

    unsigned int shard_id = (stoull(guild->get_id()) >> 22) % _client->_num_shards;
    std::shared_ptr<MainGateway> current_gateway = _client->get_shard(shard_id);

    current_gateway->send(payload).get();
    Logger("discord.voicechannel").debug("Payload with Opcode 4 (Gateway Voice State Update) has been sent");

    while (true) {
        VoiceState* voice_state = _client->get_voice_state(_client->_user->get_id(), guild->get_id());
        if ((voice_state != nullptr) && (voice_state->channel_id == get_id()) && (voice_state->endpoint.length() > 1)) {
            Logger("discord.voicechannel").debug("Creating new voice client.");

            std::shared_ptr<VoiceClient> voice_client = std::make_shared<VoiceClient>(current_gateway, voice_state->voice_token, voice_state->endpoint, voice_state->session_id, voice_state->guild_id, voice_state->channel_id, _client->_user->get_id());
            voice_state->voice_client = voice_client;
            return voice_client;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    throw ClientException("Could not connect to voice channel.");
}

std::optional<DiscordCPP::Channel> DiscordCPP::VoiceChannel::get_parent() {
    if (parent_id.has_value() && !parent.has_value()) {
        parent.emplace(Channel(parent_id.value(), get_token()));
    }
    return parent;
}
