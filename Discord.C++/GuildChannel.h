#pragma once
#include "Channel.h"
#include "Guild.h"

#include <cpprest\json.h>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class GuildChannel : public Channel {
	protected:
		Guild guild;
		string topic;
	public:
		__declspec(dllexport) GuildChannel(value data, string_t token);
		__declspec(dllexport) GuildChannel(string id, string_t token);
		__declspec(dllexport) GuildChannel();
		__declspec(dllexport) ~GuildChannel();
	};

}