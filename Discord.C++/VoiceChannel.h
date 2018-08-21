#pragma once
#include "Channel.h"

#include <cpprest/json.h>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

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

		DLL_EXPORT VoiceChannel(value data, string_t token);
		DLL_EXPORT VoiceChannel(string id, string_t token);
		DLL_EXPORT VoiceChannel(const VoiceChannel &old);
		DLL_EXPORT VoiceChannel();
		DLL_EXPORT ~VoiceChannel();
	};

}
