#pragma once
#include <iostream>

#include "DiscordObject.h"
#include "Logger.h"

namespace DiscordCPP {

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
    std::string username;
    ///the user's discord-tag
    std::string discriminator;
    ///the user's avatar hash
    std::string avatar;
    ///true, if the user is a bot
    bool bot = false;
    ///true, if the user has enabled two factor authentification
    bool mfa_enabled = false;
    ///the user's chosen language option
    std::string locale;
    ///true, if the user's email has been verified
    bool verified = false;
    ///the user's email
    std::string email;
    ///the flags on a user's account
    int flags = 0;
    ///the type of Nitro subscription on a user's account
    int premium_type = PremiumTypes::None;

    DLL_EXPORT User() {}
    DLL_EXPORT User(const json& data, const std::string& token);
    DLL_EXPORT User(const std::string& id, const std::string& token);
    DLL_EXPORT virtual ~User() {}

    ///get/create the DMChannel for this user
    DLL_EXPORT DMChannel get_dmchannel();
    ///send a message to this user
    DLL_EXPORT Message send(const std::string& content, const bool tts = false);
    ///send an embed to this user
    DLL_EXPORT Message send(const Embed& embed);

    ///@return Username as std::string
    DLL_EXPORT virtual operator std::string() { return username; };
};

}  // namespace DiscordCPP
