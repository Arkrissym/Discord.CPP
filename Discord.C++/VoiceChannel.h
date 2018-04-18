#pragma once
#include "GuildChannel.h"

#include <cpprest\json.h>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class VoiceChannel : public GuildChannel	{
	public:
		int bitrate;
		int user_limit;

		__declspec(dllexport) VoiceChannel(value data, string_t token);
		__declspec(dllexport) VoiceChannel(string id, string_t token);
		__declspec(dllexport) VoiceChannel(const VoiceChannel &old);
		__declspec(dllexport) VoiceChannel();
		__declspec(dllexport) ~VoiceChannel();

		//__declspec(dllexport) virtual VoiceChannel *copy();
	};

}