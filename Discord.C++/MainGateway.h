#pragma once
#include "Gateway.h"
#include "Intents.h"

namespace DiscordCPP {

	class MainGateway : public Gateway {
	private:
		///array of servers connected to
		std::vector<std::string> _trace;
		///session id
		std::string _session_id;
		///indicator if we have a invalid session
		bool _invalid_session;
		///current sequence number
		unsigned int _sequence_number;
		///shard id
		unsigned int _shard_id;
		///total number of shards
		unsigned int _num_shards;
		///Intents
		Intents _intents;

		DLL_EXPORT web::json::value get_heartbeat_payload();
		DLL_EXPORT void on_websocket_incoming_message(const web::json::value& payload);
		DLL_EXPORT pplx::task<void> send_heartbeat_ack();
		DLL_EXPORT pplx::task<void> identify();
	public:
		DLL_EXPORT MainGateway(const std::string& token, const Intents& intents, const int shard_id = 0, const unsigned int num_shards = 1);
		DLL_EXPORT ~MainGateway();

		DLL_EXPORT unsigned int get_shard_id();
	};

}
