#include "Message.h"
#include "static.h"

using namespace std;
using namespace web::json;
using namespace utility;

DiscordCPP::Message::Message(value data, string_t token) {
	_log = Logger("discord.message");

	if(is_valid_field("id"))
		id = conversions::to_utf8string(data.at(U("id")).as_string());
	
	//channel_id = conversions::to_utf8string(data.at(U("channel_id")).as_string());
	if (is_valid_field("channel_id"))
		channel = Channel(conversions::to_utf8string(data.at(U("channel_id")).as_string()), token);
	
	if (is_valid_field("author"))
		author = User(data.at(U("author")));

	if (is_valid_field("content"))
		content = conversions::to_utf8string(data.at(U("content")).as_string());

	
}

DiscordCPP::Message::Message() {
	_log = Logger("discord.message");
}

DiscordCPP::Message::~Message() {

}
