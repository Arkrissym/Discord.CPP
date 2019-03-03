#pragma once
#include "Gateway.h"

namespace DiscordCPP {

	class VoiceGateway : public Gateway {
		private:
			std::string _session_id;
			std::string _guild_id;
			std::string _user_id;
			bool _resume;

			DLL_EXPORT virtual web::json::value get_heartbeat_payload();
			DLL_EXPORT virtual pplx::task<void> identify();
			DLL_EXPORT virtual void on_websocket_incoming_message(web::json::value payload);
		public:
			DLL_EXPORT VoiceGateway(std::string token, std::string session_id, std::string guild_id, std::string user_id);
			DLL_EXPORT ~VoiceGateway();
	};

}
