#pragma once
#include <memory>
#include <string>

namespace DiscordCPP {

class VoiceClient;
class VoiceState {
   public:
    std::shared_ptr<VoiceClient> voice_client = nullptr;

    std::string voice_token;
    std::string endpoint;

    std::string guild_id;
    std::string channel_id;
    std::string session_id;
    // bool deaf;
    // bool mute;
    // bool self_deaf;
    // bool self_mute;
};

}  // namespace DiscordCPP
