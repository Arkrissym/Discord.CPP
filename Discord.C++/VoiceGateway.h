#pragma once
#include "Gateway.h"

namespace DiscordCPP {

	class VoiceGateway : public Gateway {
	private:
		std::string _session_id;
		std::string _guild_id;
		std::string _user_id;
		bool _resume = false;

		DLL_EXPORT virtual web::json::value get_heartbeat_payload();
		DLL_EXPORT virtual pplx::task<void> identify();
		DLL_EXPORT virtual void on_websocket_incoming_message(const web::json::value& payload);
	public:
		DLL_EXPORT VoiceGateway(const std::string& token, const std::string& session_id, const std::string& guild_id, const std::string& user_id);
		DLL_EXPORT ~VoiceGateway();
	};

}
