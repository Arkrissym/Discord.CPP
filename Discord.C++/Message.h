#pragma once
#include <iostream>
#include <cpprest\json.h>

//#include "snowflake.h"
#include "User.h"

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class Message {
	public:
		string id;	//snowflake
		string channel_id;	//snowflake
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

		__declspec(dllexport) Message(value data);
		__declspec(dllexport) Message();
	};

}