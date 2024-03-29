#pragma once
#include "Gateway.h"

namespace DiscordCPP {

class VoiceGateway : public Gateway {
   private:
    std::string _session_id;
    std::string _guild_id;
    std::string _user_id;
    bool _resume = false;

    DLL_EXPORT virtual json get_heartbeat_payload() override;
    DLL_EXPORT virtual void identify() override;
    DLL_EXPORT virtual void on_websocket_incoming_message(const std::string& message) override;

   public:
    DLL_EXPORT VoiceGateway(const std::string& token, const std::string& session_id, const std::string& guild_id, const std::string& user_id, const std::shared_ptr<Threadpool>& threadpool);
    DLL_EXPORT ~VoiceGateway(){};
};

}  // namespace DiscordCPP
