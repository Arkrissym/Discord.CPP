#include "static.h"
#include "Member.h"

using namespace std;
using namespace web::json;
using namespace utility;

DiscordCPP::Member::Member(const value& data, const string_t& token) : User(data.at(U("user")), token) {
	if (is_valid_field("nick"))
		nick = conversions::to_utf8string(data.at(U("nick")).as_string());

	//roles

	if (is_valid_field("joined_at"))
		joined_at = conversions::to_utf8string(data.at(U("joined_at")).as_string());

	if (is_valid_field("deaf"))
		deaf = data.at(U("deaf")).as_bool();

	if (is_valid_field("mute"))
		mute = data.at(U("mute")).as_bool();
}

DiscordCPP::Member::Member() {

}

DiscordCPP::Member::operator string() {
	if (nick == "")
		return username;
	else
		return nick;
}
