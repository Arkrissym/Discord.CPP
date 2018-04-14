#include "User.h"
#include "static.h"
#include "cpprest\http_client.h"

using namespace std;
using namespace web::json;
using namespace utility;
using namespace web::http;
using namespace web::http::client;

DiscordCPP::User::User() {
	_log = Logger("discord.user");
}

DiscordCPP::User::User(value data) {
	_log = Logger("discord.user");

	id = conversions::to_utf8string(data.at(U("id")).as_string());
	
	username = conversions::to_utf8string(data.at(U("username")).as_string());
	
	discriminator = conversions::to_utf8string(data.at(U("discriminator")).as_string());
	
	if (data.at(U("avatar")).is_null())
		avatar = "null";
	else
		avatar = conversions::to_utf8string(data.at(U("avatar")).as_string());

	if (data.has_field(U("bot")))
		bot = data.at(U("bot")).as_bool();

	if (data.has_field(U("mfa_enabled")))
		mfa_enabled = data.at(U("mfa_enabled")).as_bool();
	
	if (data.has_field(U("verified")))
		verified = data.at(U("verified")).as_bool();

	if ((data.has_field(U("email"))) && (!data.at(U("email")).is_null()))
		email = conversions::to_utf8string(data.at(U("email")).as_string());
}

DiscordCPP::User::User(string id, string_t token) {
	_log = Logger("discord.user");

	string url = "/users/" + id;

	http_client c(U(API_URL));
	http_request request(methods::GET);

	request.set_request_uri(uri(conversions::to_string_t(url)));
	request.headers().add(U("Authorization"), conversions::to_string_t("Bot " + conversions::to_utf8string(token)));

	c.request(request).then([this](http_response response) {
		string response_string = response.extract_utf8string().get();

		//_log.debug(response_string);

		value data = value::parse(conversions::to_string_t(response_string));

		*this = User(data);
	}).wait();
}

DiscordCPP::User::~User() {

}
