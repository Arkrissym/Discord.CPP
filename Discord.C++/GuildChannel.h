#pragma once
#include "TextChannel.h"
#include "Guild.h"

#include <cpprest\json.h>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class GuildChannel : public TextChannel {
	public:
		///the guild the channel belongs to
		Guild * guild = NULL;
		///the parent category channel
		Channel *parent;

		__declspec(dllexport) GuildChannel(value data, string_t token);
		__declspec(dllexport) GuildChannel(string id, string_t token);
		__declspec(dllexport) GuildChannel(const GuildChannel &old);
		__declspec(dllexport) GuildChannel();
		__declspec(dllexport) ~GuildChannel();
	};

}