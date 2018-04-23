#pragma once
#include <iostream>
#include <vector>

#include "DiscordObject.h"
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

	class Channel : public DiscordCPP::DiscordObject {
	public:
		int type;
		int position;
		//vector<Overwrite *> permission_overwrites;
		string name;
		bool nsfw;
		string last_message_id;	//snowflake
		vector<User *> recipients;
		string icon;
		User *owner = NULL;
		string application_id;	//snowflake
		string parent_id;
		string last_pin_timestamp;	//ISO8601 timestamp

		__declspec(dllexport) Channel(value data, string_t token);
		__declspec(dllexport) Channel(string id, string_t token);
		__declspec(dllexport) Channel(const Channel &old);
		__declspec(dllexport) Channel();
		__declspec(dllexport) ~Channel();

		template <class T>
		__declspec(dllexport) Channel *copy(T obj);
	};

	template<class T>
	inline Channel * Channel::copy(T obj) {
		T* derivedptr = new T(obj);
		Channel* baseptr = dynamic_cast<Channel*>(derivedptr);
		if (baseptr != NULL) {
			return baseptr;
		}
		// this will be reached if T is not derived from Base
		delete derivedptr;
		throw std::string("Invalid type given to Clone");
	}

}