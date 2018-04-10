#include "User.h"

using namespace std;
using namespace web::json;
using namespace utility;

DiscordCPP::User::User() {

}

DiscordCPP::User::User(value data) {
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
