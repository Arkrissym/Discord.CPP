#pragma once
#include <iostream>
#include <vector>
#include <cpprest\json.h>

#include "Logger.h"

#include "User.h"

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	namespace ChannelType {
		enum ChannelType {
			GUILD_TEXT,
			DM,
			GUILD_VOICE,
			GROUP_DM,
			GUILD_CATEGORY
		};
	}

	class Channel {
	protected:
		Logger _log;
	public:
		string id;	//snowflake
		int type;
		int position = 0;
		//vector<Overwrite> permission_overwrites = NULL;
		string name;
		bool nsfw = false;
		string last_message_id;	//snowflake
		vector<User> recipients;
		string icon;
		User owner;
		string application_id;	//snowflake
		string parent_id;
		string last_pin_timestamp;	//ISO8601 timestamp

		__declspec(dllexport) Channel(value data, string_t token);
		__declspec(dllexport) Channel(string id, string_t token);
		__declspec(dllexport) Channel();
		__declspec(dllexport) ~Channel();
	};

}