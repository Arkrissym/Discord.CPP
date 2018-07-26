#pragma once
#include "Channel.h"
//#include "Message.h"
//#include "Embed.h"

#include <cpprest\json.h>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class Message;
	class Embed;

	class TextChannel : public Channel {
	public:
		///the topic of the channel
		string topic;

		__declspec(dllexport) TextChannel(value data, string_t token);
		__declspec(dllexport) TextChannel(string id, string_t token);
		__declspec(dllexport) TextChannel(const TextChannel &old);
		__declspec(dllexport) TextChannel();
		__declspec(dllexport) ~TextChannel();

		///Send a message to this channel.
		__declspec(dllexport) Message send(string content, bool tts=false);
		__declspec(dllexport) Message send(Embed embed);
	};

}