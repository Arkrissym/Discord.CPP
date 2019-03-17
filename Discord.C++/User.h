#pragma once
#include "DiscordObject.h"
//#include "Message.h"
//#include "Embed.h"
//#include "DMChannel.h"

#include <iostream>

#include "Logger.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;
	using namespace utility;

	class Message;
	class Embed;
	class DMChannel;

	class User : public DiscordObject {
	public:
		enum PremiumTypes {
			None,
			NitroClassic,
			Nitro
		};

		///ther user's name
		string username;
		///the user's discord-tag
		string discriminator;
		///the user's avatar hash
		string avatar;
		///true, if the user is a bot
		bool bot = false;
		///true, if the user has enabled two factor authentification
		bool mfa_enabled = false;
		///the user's chosen language option
		string locale;
		///true, if the user's email has been verified
		bool verified = false;
		///the user's email
		string email;
		///the flags on a user's account
		int flags = 0;
		///the type of Nitro subscription on a user's account
		int premium_type = PremiumTypes::None;

		DLL_EXPORT User();
		DLL_EXPORT User(value data, string_t token);
		DLL_EXPORT User(string id, string_t token);
		
		///get/create the DMChannel for this user
		DLL_EXPORT DMChannel get_dmchannel();
		///send a message to this user
		DLL_EXPORT Message send(string content, bool tts = false);
		///send an embed to this user
		DLL_EXPORT Message send(Embed embed);

		///@return Username as std::string
		DLL_EXPORT virtual operator string() { return username; };
	};

}
