#pragma once

#include <cpprest/json.h>

namespace DiscordCPP {

	using namespace utility;

	class VoiceState {
	public:
		string_t voice_token;
		string_t endpoint;

		string_t guild_id;
		string_t channel_id;
		string_t session_id;
		//bool deaf;
		//bool mute;
		//bool self_deaf;
		//bool self_mute;

		VoiceState();
		~VoiceState();
	};

}