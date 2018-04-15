#include "Message.h"
#include "static.h"

using namespace std;
using namespace web::json;
using namespace utility;

DiscordCPP::Message::Message(value data, string_t token) {
	_log = Logger("discord.message");

	if(is_valid_field("id"))
		id = conversions::to_utf8string(data.at(U("id")).as_string());
	
	if (is_valid_field("channel_id"))
		channel = Channel(conversions::to_utf8string(data.at(U("channel_id")).as_string()), token);
	
	if (is_valid_field("author"))
		author = User(data.at(U("author")));

	if (is_valid_field("content"))
		content = conversions::to_utf8string(data.at(U("content")).as_string());

	if (is_valid_field("timestamp"))
		timestamp = conversions::to_utf8string(data.at(U("timestamp")).as_string());

	if (is_valid_field("edited_timestamp"))
		edited_timestamp = conversions::to_utf8string(data.at(U("edited_timestamp")).as_string());

	if (is_valid_field("tts"))
		tts = data.at(U("tts")).as_bool();

	if (is_valid_field("mention_everyone"))
		tts = data.at(U("mention_everyone")).as_bool();

	if (is_valid_field("mentions")) {
		web::json::array tmp = data.at(U("mentions")).as_array();
		for (int i = 0; i < tmp.size(); i++)
			mentions.push_back(User(tmp[i]));
	}

	//mention_roles

	//attachements

	//embeds

	//reactions

	if (is_valid_field("pinned"))
		pinned = data.at(U("pinned")).as_bool();

	if (is_valid_field("webhook_id"))
		webhook_id = conversions::to_utf8string(data.at(U("webhook_id")).as_string());

	if (is_valid_field("type"))
		type = data.at(U("type")).as_integer();

	//activity

	//application

	//_log.debug("created message object");
}

DiscordCPP::Message::Message() {
	_log = Logger("discord.message");
	//_log.debug("created empty message object");
}

DiscordCPP::Message::~Message() {
	//_log.debug("destroyed message object");
}
