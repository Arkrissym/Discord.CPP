#pragma once
#include <iostream>
#include <vector>

#include "DiscordObject.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

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
			GUILD_CATEGORY,
			GUILD_NEWS
		};
	}

	class User;
	class Discord;

	class Channel : public DiscordObject {
	public:
		///the ChannelType of the channel
		int type;
		///the sorting position
		int position;
		//vector<Overwrite *> permission_overwrites;
		///the channel's name
		string name;
		///the channel's icon hash
		string icon;

		DLL_EXPORT Channel(const value& data, const string_t& token);
		DLL_EXPORT Channel(const string& id, const string_t& token);
		DLL_EXPORT Channel(const Channel& old);
		DLL_EXPORT Channel();

		DLL_EXPORT static Channel* from_json(Discord* client, const value& data, const string_t& token);

		///Delete this channel
		DLL_EXPORT void delete_channel();

		template <class T>
		DLL_EXPORT Channel* copy(T obj);

		///@return Channelname as std::string
		DLL_EXPORT operator string() { return name; };
	};

	template<class T>
	inline Channel* Channel::copy(T obj) {
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
