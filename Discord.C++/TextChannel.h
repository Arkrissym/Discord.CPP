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

	class Message;
	class Embed;

	class TextChannel : public Channel {
	public:
		///the topic of the channel
		string topic;
		///true if nsfw is enabled
		bool nsfw;
		///the id of the last message
		string last_message_id;	//snowflake
		///when the last message was pinned
		string last_pin_timestamp;	//ISO8601 timestamp
		///amount of seconds a User has to wait before sending another message(0-120). Bots, as well as Users with MANAGE_MESSAGES or MANAGE_CHANNELS Permissions are unaffected.
		int rate_limit_per_user = 0;

		DLL_EXPORT TextChannel(const value& data, const string_t& token);
		DLL_EXPORT TextChannel(const string& id, const string_t& token);
		DLL_EXPORT TextChannel(const TextChannel& old);
		DLL_EXPORT TextChannel();

		///Send a Message to this channel.
		DLL_EXPORT Message send(const string& content, const bool tts = false);
		DLL_EXPORT Message send(Embed embed);

		///Get messages from channel history
		DLL_EXPORT vector<shared_ptr<Message>> history(const int limit = 100, const string& before = "", const string& after = "", const string& around = "");

		///Delete multiple messages from this channel
		DLL_EXPORT void delete_messages(const vector<shared_ptr<Message>>& messages);
	};

}
