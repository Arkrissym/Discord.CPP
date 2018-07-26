#include "DMChannel.h"
#include "User.h"
#include "static.h"

using namespace std;
using namespace web::json;
using namespace utility;

DiscordCPP::DMChannel::DMChannel(value data, string_t token) : DiscordCPP::TextChannel(data, token) {
	if (is_valid_field("recipients")) {
		//recipients = new vector<User *>;
		web::json::array tmp = data.at(U("recipients")).as_array();
		for (int i = 0; i < tmp.size(); i++)
			recipients.push_back(new User(tmp[i], token));
	}

	if (is_valid_field("owner_id"))
		owner = new User(conversions::to_utf8string(data.at(U("owner_id")).as_string()), token);

	if (is_valid_field("application_id"))
		application_id = conversions::to_utf8string(data.at(U("application_id")).as_string());
}

DiscordCPP::DMChannel::DMChannel(string id, string_t token) {
	string url = "/channels/" + id;
	_token = token;
	*this = DMChannel(api_call(url), token);
}

DiscordCPP::DMChannel::DMChannel(const DMChannel & old) : DiscordCPP::TextChannel(old) {
	for (int i = 0; i < old.recipients.size(); i++) {
		recipients.push_back(new User(*old.recipients[i]));
	}
	if (old.owner != NULL)
		owner = new User(*old.owner);
	application_id = old.application_id;
}

DiscordCPP::DMChannel::~DMChannel() {
	for (int i = 0; i < recipients.size(); i++) {
		delete recipients[i];
	}
	delete owner;
}