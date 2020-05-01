#include "static.h"
#include "Guild.h"
#include "VoiceChannel.h"
#include "DMChannel.h"
#include "TextChannel.h"

#include <cpprest/http_client.h>

using namespace std;
using namespace web::json;
using namespace utility;
using namespace web::http;
using namespace web::http::client;

DiscordCPP::Guild::Guild(Discord* client, const value& data, const string_t& token) : DiscordCPP::DiscordObject(token) {
	if (is_valid_field("id"))
		id = conversions::to_utf8string(data.at(U("id")).as_string());

	if (is_valid_field("name"))
		name = conversions::to_utf8string(data.at(U("name")).as_string());

	if (is_valid_field("icon"))
		icon = conversions::to_utf8string(data.at(U("icon")).as_string());

	if (is_valid_field("splash"))
		splash = conversions::to_utf8string(data.at(U("splash")).as_string());

	if (is_valid_field("owner_id"))
		owner = new User(conversions::to_utf8string(data.at(U("owner_id")).as_string()), token);

	if (is_valid_field("permissions"))
		permissions = data.at(U("permissions")).as_integer();

	if (is_valid_field("region"))
		region = conversions::to_utf8string(data.at(U("region")).as_string());

	if (is_valid_field("afk_channel_id")) {
		afk_channel = new VoiceChannel(client, conversions::to_utf8string(data.at(U("afk_channel_id")).as_string()), token);
	}

	if (is_valid_field("afk_timeout"))
		afk_timeout = data.at(U("afk_timeout")).as_integer();

	if (is_valid_field("embed_enabled"))
		embed_enabled = data.at(U("embed_enabled")).as_bool();

	if (is_valid_field("embed_channel_id")) {
		embed_channel = new TextChannel(conversions::to_utf8string(data.at(U("embed_channel_id")).as_string()), token);
	}

	if (is_valid_field("verfication_level"))
		afk_timeout = data.at(U("verification_level")).as_integer();

	if (is_valid_field("default_message_notifications"))
		default_message_notifications = data.at(U("default_message_notifications")).as_integer();

	if (is_valid_field("explicit_content_filter"))
		explicit_content_filter = data.at(U("explicit_content_filter")).as_integer();

	//roles

	//emojis

	if (is_valid_field("features")) {
		web::json::array tmp = data.at(U("features")).as_array();
		for (unsigned int i = 0; i < tmp.size(); i++)
			features.push_back(conversions::to_utf8string(tmp[i].as_string()));
	}

	if (is_valid_field("mfa_level"))
		mfa_level = data.at(U("mfa_level")).as_integer();

	if (is_valid_field("application_id"))
		application_id = conversions::to_utf8string(data.at(U("application_id")).as_string());

	if (is_valid_field("widget_enabled"))
		widget_enabled = data.at(U("widget_enabled")).as_bool();

	if (is_valid_field("widget_channel_id")) {
		widget_channel = new Channel(conversions::to_utf8string(data.at(U("widget_channel_id")).as_string()), token);
	}

	if (is_valid_field("system_channel_id")) {
		system_channel = new TextChannel(conversions::to_utf8string(data.at(U("system_channel_id")).as_string()), token);
	}

	if (is_valid_field("joined_at"))
		joined_at = conversions::to_utf8string(data.at(U("joined_at")).as_string());

	if (is_valid_field("large"))
		large = data.at(U("large")).as_bool();

	if (is_valid_field("unavailable"))
		unavailable = data.at(U("unavailable")).as_bool();

	if (is_valid_field("member_count"))
		member_count = data.at(U("member_count")).as_integer();

	//voice_states

	if (is_valid_field("members")) {
		web::json::array tmp = data.at(U("members")).as_array();
		for (unsigned int i = 0; i < tmp.size(); i++)
			members.push_back(new Member(tmp[i], token));
	}

	if (is_valid_field("channels")) {
		web::json::array tmp = data.at(U("channels")).as_array();
		for (unsigned int i = 0; i < tmp.size(); i++) {
			channels.push_back(Channel::from_json(client, tmp[i], token));
		}
	}

	//presences
}

DiscordCPP::Guild::Guild(Discord* client, const string& id, const string_t& token) : DiscordCPP::DiscordObject(token) {
	string url = "/guilds/" + id;

	*this = Guild(client, api_call(url), token);
}

DiscordCPP::Guild::Guild(const Guild& old) {
	_token = old._token;
	id = old.id;
	name = old.name;
	icon = old.icon;
	splash = old.splash;
	if (old.owner != NULL)
		owner = new User(*old.owner);
	permissions = old.permissions;
	region = old.region;
	if (old.afk_channel != NULL)
		afk_channel = old.afk_channel->copy(*old.afk_channel);
	afk_timeout = old.afk_timeout;
	embed_enabled = old.embed_enabled;
	if (old.embed_channel != NULL)
		embed_channel = old.embed_channel->copy(*old.embed_channel);
	verification_level = old.verification_level;
	default_message_notifications = old.default_message_notifications;
	explicit_content_filter = old.explicit_content_filter;
	//roles
	//emojis
	features = old.features;
	mfa_level = old.mfa_level;
	application_id = old.application_id;
	widget_enabled = old.widget_enabled;
	if (old.widget_channel != NULL)
		widget_channel = old.widget_channel->copy(*old.widget_channel);
	if (old.system_channel != NULL)
		system_channel = old.system_channel->copy(*old.system_channel);
	joined_at = old.joined_at;
	large = old.large;
	unavailable = old.unavailable;
	member_count = old.member_count;
	//voice_states
	for (unsigned int i = 0; i < old.members.size(); i++) {
		members.push_back(new Member(*old.members[i]));
	}
	for (unsigned int i = 0; i < old.channels.size(); i++) {
		channels.push_back(old.channels[i]->copy(*old.channels[i]));
	}
	//presences
}

DiscordCPP::Guild::Guild() {

}

DiscordCPP::Guild::~Guild() {
	if (owner != NULL)
		delete owner;
	if (afk_channel != NULL)
		delete afk_channel;
	if (embed_channel != NULL)
		delete embed_channel;
	if (widget_channel != NULL)
		delete widget_channel;
	if (system_channel != NULL)
		delete system_channel;
	for (unsigned int i = 0; i < members.size(); i++) {
		delete members[i];
	}
	for (unsigned int i = 0; i < channels.size(); i++) {
		delete channels[i];
	}
}

void DiscordCPP::Guild::_add_channel(Channel* channel) {
	channels.push_back(channel);
}

void DiscordCPP::Guild::_update_channel(Channel* channel) {
	for (size_t i = 0; i < channels.size(); i++) {
		if (channels[i]->id == channel->id) {
			delete channels[i];
			channels.erase(channels.begin() + i);
			channels.push_back(channel);
			break;
		}
	}
}

void DiscordCPP::Guild::_remove_channel(const string& channel_id) {
	for (size_t i = 0; i < channels.size(); i++) {
		if (channels[i]->id == channel_id) {
			delete channels[i];
			channels.erase(channels.begin() + i);
			break;
		}
	}
}

void DiscordCPP::Guild::_add_member(Member* member) {
	members.push_back(member);
	member_count += 1;
}

void DiscordCPP::Guild::_update_member(Member* member) {
	for (size_t i = 0; i < members.size(); i++) {
		if (members[i]->id == member->id) {
			delete members[i];
			members.erase(members.begin() + i);
			members.push_back(member);
		}
	}
}

void DiscordCPP::Guild::_remove_member(const string& member_id) {
	for (size_t i = 0; i < members.size(); i++) {
		if (members[i]->id == member_id) {
			delete members[i];
			members.erase(members.begin() + i);
			member_count -= 1;
		}
	}
}

///@throws HTTPError
void DiscordCPP::Guild::leave() {
	string url = "/guilds/@me/guilds/" + id;
	api_call(url, methods::DEL);
}

///@throws HTTPError
void DiscordCPP::Guild::delete_guild() {
	string url = "/guilds/" + id;
	api_call(url, methods::DEL);
}

/**	@param[in]	user	User to kick
	@throws HTTPError
*/
void DiscordCPP::Guild::kick(const User& user) {
	string url = "/guilds/" + id + "/members/" + user.id;
	api_call(url, methods::DEL);
}

/**	@param[in]	user				User to ban
	@param[in]	reason				reason, why the User should be banned
	@param[in]	delete_message_days	(optional) number of days to delete messages from user (0-7)
	@throws HTTPError
*/
void DiscordCPP::Guild::ban(const User& user, const string& reason, const int delete_message_days) {
	string url = "/guilds/" + id + "/bans/" + user.id + "?delete-message-days=" + to_string(delete_message_days) + "&reason=" + urlencode(reason);
	api_call(url, methods::PUT);
}

/**	@param[in]	user	User to kick
	@throws HTTPError
*/
void DiscordCPP::Guild::unban(const User& user) {
	string url = "/guilds/" + id + "/bans/" + user.id;
	api_call(url, methods::DEL);
}
