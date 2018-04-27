#pragma once
#include "Channel.h"

#include <cpprest\json.h>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class VoiceChannel : public Channel	{
	public:
		///the channel's bitrate
		int bitrate;
		///the channel's user-limit
		int user_limit;
		///the parent category channel
		Channel *parent;

		__declspec(dllexport) VoiceChannel(value data, string_t token);
		__declspec(dllexport) VoiceChannel(string id, string_t token);
		__declspec(dllexport) VoiceChannel(const VoiceChannel &old);
		__declspec(dllexport) VoiceChannel();
		__declspec(dllexport) ~VoiceChannel();
	};

}