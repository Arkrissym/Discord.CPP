#pragma once
#include "TextChannel.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class DMChannel : public TextChannel {
	public:
		///the recipients if the channel is DM
		vector<User*> recipients;
		///the owner of this DM channel
		User* owner = NULL;
		///the application id of the group DM creator if it was bot-created
		string application_id;	//snowflake

		DLL_EXPORT DMChannel(const value& data, const string_t& token);
		DLL_EXPORT DMChannel(const string& id, const string_t& token);
		DLL_EXPORT DMChannel(const DMChannel& old);
		DLL_EXPORT ~DMChannel();
	};

}
