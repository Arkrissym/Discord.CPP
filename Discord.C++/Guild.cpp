#include "static.h"
#include "Guild.h"
#include "GuildChannel.h"
#include "VoiceChannel.h"

#include <cpprest\http_client.h>

using namespace std;
using namespace web::json;
using namespace utility;
using namespace web::http;
using namespace web::http::client;

DiscordCPP::Guild::Guild(value data, string_t token) : DiscordCPP::DiscordObject(token) {
	//_log = Logger("dicord.guild");

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
		if (is_valid_field("bitrate")) {
			afk_channel = new VoiceChannel(conversions::to_utf8string(data.at(U("afk_channel_id")).as_string()), token);
		}
		else if (is_valid_field("topic")) {
			afk_channel = new GuildChannel(conversions::to_utf8string(data.at(U("afk_channel_id")).as_string()), token);
		}
		else {
			afk_channel = new Channel(conversions::to_utf8string(data.at(U("afk_channel_id")).as_string()), token);
		}
	}

	if (is_valid_field("afk_timeout"))
		afk_timeout = data.at(U("afk_timeout")).as_integer();

	if (is_valid_field("embed_enabled"))
		embed_enabled = data.at(U("embed_enabled")).as_bool();

	if (is_valid_field("embed_channel_id")) {
		if (is_valid_field("bitrate")) {
			embed_channel = new VoiceChannel(conversions::to_utf8string(data.at(U("embed_channel_id")).as_string()), token);
		}
		else if (is_valid_field("topic")) {
			embed_channel = new GuildChannel(conversions::to_utf8string(data.at(U("embed_channel_id")).as_string()), token);
		}
		else {
			embed_channel = new Channel(conversions::to_utf8string(data.at(U("embed_channel_id")).as_string()), token);
		}
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
		for (int i = 0; i < tmp.size(); i++)
			features.push_back(conversions::to_utf8string(tmp[i].as_string()));
	}

	if (is_valid_field("mfa_level"))
		mfa_level = data.at(U("mfa_level")).as_integer();

	if (is_valid_field("application_id"))
		application_id = conversions::to_utf8string(data.at(U("application_id")).as_string());

	if (is_valid_field("widget_enabled"))
		widget_enabled = data.at(U("widget_enabled")).as_bool();

	if (is_valid_field("widget_channel_id")) {
		if (is_valid_field("bitrate")) {
			widget_channel = new VoiceChannel(conversions::to_utf8string(data.at(U("widget_channel_id")).as_string()), token);
		}
		else if (is_valid_field("topic")) {
			widget_channel = new GuildChannel(conversions::to_utf8string(data.at(U("widget_channel_id")).as_string()), token);
		}
		else {
			widget_channel = new Channel(conversions::to_utf8string(data.at(U("widget_channel_id")).as_string()), token);
		}
	}

	if (is_valid_field("system_channel_id")) {
		if (is_valid_field("bitrate")) {
			system_channel = new VoiceChannel(conversions::to_utf8string(data.at(U("system_channel_id")).as_string()), token);
		}
		else if (is_valid_field("topic")) {
			system_channel = new GuildChannel(conversions::to_utf8string(data.at(U("system_channel_id")).as_string()), token);
		}
		else {
			system_channel = new Channel(conversions::to_utf8string(data.at(U("system_channel_id")).as_string()), token);
		}
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
		for (int i = 0; i < tmp.size(); i++)
			members.push_back(new Member(tmp[i], token));
	}

	if (is_valid_field("channels")) {
		web::json::array tmp = data.at(U("channels")).as_array();
		for (int i = 0; i < tmp.size(); i++) {
			if ((tmp[i].has_field(U("bitrate"))) && (!tmp[i].at(U("bitrate")).is_null())) {
				channels.push_back(new VoiceChannel(tmp[i], token));
			}
			else if ( (tmp[i].has_field(U("topic"))) && (!tmp[i].at(U("topic")).is_null()) ) {
				channels.push_back(new GuildChannel(tmp[i], token));
			}
			else {
				channels.push_back(new Channel(tmp[i], token));
			}
		}
	}

	//presences

	//_log.debug("created guild object");
}

DiscordCPP::Guild::Guild(string id, string_t token) : DiscordCPP::DiscordObject(token) {
	//_log = Logger("discord.guild");

	string url = "/guilds/" + id;

	/*http_client c(U(API_URL));
	http_request request(methods::GET);

	request.set_request_uri(uri(conversions::to_string_t(url)));
	request.headers().add(U("Authorization"), conversions::to_string_t("Bot " + conversions::to_utf8string(token)));

	c.request(request).then([this, token](http_response response) {
		string response_string = response.extract_utf8string().get();

		//_log.debug(response_string);

		value data = value::parse(conversions::to_string_t(response_string));

		*this = Guild(data, token);
	}).wait();*/

	*this = Guild(api_call(url), token);
}

DiscordCPP::Guild::Guild(const Guild & old) {
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
	for (int i = 0; i < old.members.size(); i++) {
		members.push_back(new Member(*old.members[i]));
	}
	for (int i = 0; i < old.channels.size(); i++) {
		channels.push_back(old.channels[i]->copy(*old.channels[i]));
	}
	//presences
}

DiscordCPP::Guild::Guild() {
	//_log = Logger("dicord.guild");
	//_log.debug("created empty guild object");
}

DiscordCPP::Guild::~Guild() {
	//_log.debug("destroyed guild object");

	delete owner;
	delete afk_channel;
	delete embed_channel;
	delete widget_channel;
	delete embed_channel;
	for (int i = 0; i < members.size(); i++) {
		delete members[i];
	}
	for (int i = 0; i < channels.size(); i++) {
		delete channels[i];
	}
}