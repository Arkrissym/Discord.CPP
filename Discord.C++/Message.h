#pragma once
#include <iostream>
#include <cpprest\json.h>

#include "Logger.h"

//#include "snowflake.h"
#include "User.h"
#include "Channel.h"

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class Message {
	protected:
		Logger _log;
	public:
		string id;	//snowflake
		Channel channel;
		User author;
		string content;
		string timestamp;	//ISO8601 timestamp
		string edited_timestamp;	//ISO8601 timestamp
		bool tts;
		bool mention_everyone;
		vector<User> mentions;
		//vector<Role> mention_roles;
		//vector<Attachments> attachments;
		//vector<Embed> embeds;
		//vector<Reaction> reactions;
		bool pinned;
		string webhook_id;	//snowflake
		int type;
		//MessageAcivity activity;
		//MessageApplication application;

		__declspec(dllexport) Message(value data, string_t token);
		__declspec(dllexport) Message();
		__declspec(dllexport) ~Message();
	};

}