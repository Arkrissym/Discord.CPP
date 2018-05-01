#include "TextChannel.h"
#include "static.h"

using namespace std;
using namespace web::json;
using namespace utility;

DiscordCPP::TextChannel::TextChannel(value data, string_t token) : DiscordCPP::Channel(data, token) {
	if (is_valid_field("topic"))
		topic = conversions::to_utf8string(data.at(U("topic")).as_string());
}

DiscordCPP::TextChannel::TextChannel(string id, string_t token) {
	string url = "/channels/" + id; 
	_token = token;
	*this = TextChannel(api_call(url), token);
}

DiscordCPP::TextChannel::TextChannel(const TextChannel & old) : DiscordCPP::Channel(old) {
	topic = old.topic;
}

DiscordCPP::TextChannel::TextChannel() {

}

DiscordCPP::TextChannel::~TextChannel() {

}

/**	@param[in]	content	The string message to send.
	@param[in]	tts		(optional) Wether to send as tts-message or not. Default is false.
	@return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::TextChannel::send(string content, bool tts) {
	string url = "/channels/" + id + "/messages";

	value data;
	data[U("content")] = value(conversions::to_string_t(content));
	data[U("tts")] = value(tts);

	return Message(api_call(url, methods::POST, data), _token);
}
