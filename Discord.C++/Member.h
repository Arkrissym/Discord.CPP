#pragma once
//#include "Logger.h"
#include "User.h"

#include <iostream>
#include <cpprest/json.h>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class Member : public User {
	public:
		///the member's nickname
		string nick;
		//vector<Role> roles;
		///when the member joined the server
		string joined_at;	//ISO8601 timestamp
		///true, if the member is defeaned
		bool deaf = false;
		///true, if the member is muted
		bool mute = false;

		DLL_EXPORT Member(value data, string_t token);
		DLL_EXPORT Member();
		DLL_EXPORT ~Member();

		///@return Nickname (Username if not present) as std::string
		DLL_EXPORT operator string();
	};

}
