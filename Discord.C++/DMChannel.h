#pragma once
#include "Channel.h"

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class DMChannel : public Channel {
	public:
		///the recipients if the channel is DM
		vector<User *> recipients;
		///the owner of this DM channel
		User *owner = NULL;
		///the application id of the group DM creator if it was bot-created
		string application_id;	//snowflake

		__declspec(dllexport) DMChannel(value data, string_t token);
		__declspec(dllexport) DMChannel(string id, string_t token);
		__declspec(dllexport) DMChannel(const DMChannel & old);
		__declspec(dllexport) ~DMChannel();
	};

}