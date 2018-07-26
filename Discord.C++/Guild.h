#pragma once
#include "DiscordObject.h"
#include "User.h"
#include "Channel.h"
#include "Member.h"

#include <iostream>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	namespace VerificationLevel {
		enum VerificationLevel {
			NONE,
			LOW,
			MEDIUM,
			HIGH,
			VERY_HIGH
		};
	}

	namespace DefaultMessageNotificationLevel {
		enum DefaultMessageNotificationLevel {
			ALL_MESSAGES,
			ONLY_MENTIONS
		};
	}

	namespace ExplicitContentFilterLevel {
		enum ExplicitContentFilterLevel {
			DISABLED,
			MEMBERS_WITHOUT_ROLES,
			ALL_MEMBERS
		};
	}

	namespace MFALevel {
		enum MFALevel {
			NONE,
			ELEVATED
		};
	}

	class User;
	class Channel;
	class Member;

	class Guild : public DiscordCPP::DiscordObject {
	public:
		///the guild's name
		string name;
		///the guild's icon hash
		string icon;
		///the guild's splash hash
		string splash;
		///the owner of the guild
		User *owner = NULL;
		///total permissions for the user(doesn't include channel overrides)
		int permissions = 0;
		///the guild's voice redion
		string region;
		///the guild's afk channel(voice)
		Channel *afk_channel = NULL;
		///afk timeout in seconds
		int afk_timeout = 0;
		///true, if the guild is embeddable
		bool embed_enabled = false;
		///the embedded channel
		Channel *embed_channel = NULL;
		///the VerificationLevel required to join the guild
		int verification_level = 0;
		///DefaultMessageNotificationLevel
		int default_message_notifications = 0;
		///ExplicitContentFilterLevel
		int explicit_content_filter = 0;
		//vector<Role> roles;
		//vector<Emoji> emojis;
		///enabled guild features
		vector<string> features;
		///MFALevel
		int mfa_level = 0;
		///application id of the guild creator if the guild is bot-created
		string application_id;	//snowflake
		///true if the server widget is enabled
		bool widget_enabled = false;
		///the channel for the server widget
		Channel *widget_channel = NULL;
		///the channel to which system messages are sent
		Channel *system_channel = NULL;

		//GUILD_CREATE event only
		///when the guild was created
		string joined_at;	//ISO8601 timestamp
		///true if it is a large guild
		bool large = false;
		///true if the guild is not available
		bool unavailable = false;
		///number of members
		int member_count = 0;
		//vector<VoiceState> voice_states;
		///users in the guild
		vector<Member *> members;
		///channels of the guild
		vector<Channel *> channels;
		//vector<Presence> presences;

		__declspec(dllexport) Guild(value data, string_t token);
		__declspec(dllexport) Guild(string id, string_t token);
		__declspec(dllexport) Guild(const Guild &old);
		__declspec(dllexport) Guild();
		__declspec(dllexport) ~Guild();

		///@return Guildname as std::string
		__declspec(dllexport) operator string() { return name; };
	};

}