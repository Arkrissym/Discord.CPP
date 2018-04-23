#pragma once
#include "DiscordObject.h"
#include "User.h"
#include "Channel.h"
#include "Member.h"

#include <iostream>
//#include <cpprest\json.h>

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

	class Guild : public DiscordCPP::DiscordObject {
	//protected:
	//	Logger _log;
	public:
		//string id;	//snowflake
		string name;
		string icon;
		string splash;
		User *owner = NULL;
		int permissions = 0;
		string region;
		Channel *afk_channel = NULL;
		int afk_timeout = 0;
		bool embed_enabled = false;
		Channel *embed_channel = NULL;
		int verification_level = 0;
		int default_message_notifications = 0;
		int explicit_content_filter = 0;
		//vector<Role> roles;
		//vector<Emoji> emojis;
		vector<string> features;
		int mfa_level = 0;
		string application_id;	//snowflake
		bool widget_enabled = false;
		Channel *widget_channel = NULL;
		Channel *system_channel = NULL;

		//GUILD_CREATE event only
		string joined_at;	//ISO8601 timestamp
		bool large = false;
		bool unavailable = false;
		int member_count = 0;
		//vector<VoiceState> voice_states;
		vector<Member *> members;
		vector<Channel *> channels;
		//vector<Presence> presences;

		__declspec(dllexport) Guild(value data, string_t token);
		__declspec(dllexport) Guild(string id, string_t token);
		__declspec(dllexport) Guild(const Guild &old);
		__declspec(dllexport) Guild();
		__declspec(dllexport) ~Guild();
	};

}