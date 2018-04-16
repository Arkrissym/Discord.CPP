#include "Channel.h"
#include "static.h"

#include <cpprest\http_client.h>

using namespace std;
using namespace web::json;
using namespace utility;
using namespace web::http;
using namespace web::http::client;

DiscordCPP::Channel::Channel(value data, string_t token) {
	_log = Logger("discord.channel");

	if (is_valid_field("id"))
		id = conversions::to_utf8string(data.at(U("id")).as_string());

	if (is_valid_field("type"))
		type = data.at(U("type")).as_integer();

	if (is_valid_field("position"))
		position = data.at(U("position")).as_integer();

	//permission_overwrites

	if (is_valid_field("name"))
		name = conversions::to_utf8string(data.at(U("name")).as_string());

	if (is_valid_field("nsfw"))
		position = data.at(U("nsfw")).as_bool();

	if (is_valid_field("last_message_id"))
		last_message_id = conversions::to_utf8string(data.at(U("last_message_id")).as_string());

	if (is_valid_field("recipients")) {
		web::json::array tmp = data.at(U("recipients")).as_array();
		for (int i = 0; i < tmp.size(); i++)
			recipients.push_back(User(tmp[i]));
	}
	
	if (is_valid_field("icon"))
		icon = conversions::to_utf8string(data.at(U("icon")).as_string());

	if (is_valid_field("owner_id"))
		owner = User(conversions::to_utf8string(data.at(U("owner_id")).as_string()), token);

	if (is_valid_field("application_id"))
		application_id = conversions::to_utf8string(data.at(U("application_id")).as_string());

	//parent

	if (is_valid_field("last_pin_timestamp"))
		last_pin_timestamp = conversions::to_utf8string(data.at(U("last_pin_timestamp")).as_string());

	//_log.debug("created channel object");
}

DiscordCPP::Channel::Channel(string id, string_t token) {
	_log = Logger("discord.channel");

	string url = "/channels/" + id;

	http_client c(U(API_URL));
	http_request request(methods::GET);

	request.set_request_uri(uri(conversions::to_string_t(url)));
	request.headers().add(U("Authorization"), conversions::to_string_t("Bot " + conversions::to_utf8string(token)));

	Concurrency::task<void> requestTask = c.request(request).then([this, token](http_response response) {
		string response_string = response.extract_utf8string().get();

		//_log.debug(response_string);

		value data = value::parse(conversions::to_string_t(response_string));

		*this = Channel(data, token);
	});

	try {
		requestTask.wait();
	}
	catch (const std::exception &e) {
		_log.error("Error exception: " + string(e.what()));
	}
}

DiscordCPP::Channel::Channel() {
	_log = Logger("discord.channel");
	//_log.debug("created empty channel object");
}

DiscordCPP::Channel::~Channel() {
	//_log.debug("destroyed channel object");
}
