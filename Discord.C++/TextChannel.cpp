#include "TextChannel.h"
#include "Message.h"
#include "Embed.h"
#include "static.h"
#include "Exceptions.h"

using namespace std;
using namespace web::json;
using namespace web::http;
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

/**	@param[in]	embed	The Embed to send.
	@return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::TextChannel::send(DiscordCPP::Embed embed) {
	string url = "/channels/" + id + "/messages";

	value data;
	data[U("embed")] = embed.to_json();
	return Message(api_call(url, methods::POST, data), _token);
}

/**	@param[in]	limit	Max number of messages to retrieve (1-100)
	@param[in]	before	Get messages before this message id
	@param[in]	after	Get messages after this message id
	@param[in]	around	Get messages around this message id
	@return	Array of messages
*/
vector<shared_ptr<DiscordCPP::Message>> DiscordCPP::TextChannel::history(int limit, string before, string after, string around) {
	vector<shared_ptr<Message>> ret;
	
	string url = "/channels/" + id + "/messages" + "?limit=" + to_string(limit);

	if (before.length() > 0)
		url += "&before=" + before;
	if (after.length() > 0)
		url += "&after=" + after;
	if (around.length() > 0)
		url += "&around" + around;

	web::json::array msgs = api_call(url, methods::GET, value(), "", false).as_array();

	for (unsigned int i = 0; i < msgs.size(); i++) {
		//cout << endl << conversions::to_utf8string(msgs[i].serialize());
		ret.push_back(make_shared<Message>(Message(msgs[i], _token)));
	}

	return ret;
}

/**	@param[in]	messages	vector containing the messages to delete
	@throws	SizeError
*/
void DiscordCPP::TextChannel::delete_messages(vector<shared_ptr<Message>> messages) {
	if (messages.size() < 2)
		throw SizeError("Cannot delete less than 2 messages: use Message::delete_msg() instead");
	else if (messages.size() > 100)
		throw SizeError("Cannot delete more than 100 messages");

	string url = "/channels/" + id + "/messages/bulk-delete";

	value data;
	data[U("messages")] = value();
	for (unsigned int i = 0; i < messages.size(); i++) {
		data[U("messages")][i] = value(conversions::to_string_t(messages[i]->id));
	}

	api_call(url, methods::POST, data);
}
