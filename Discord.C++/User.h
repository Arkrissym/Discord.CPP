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

   private:
    /// the user's name
    std::string username;
    /// the user's discord-tag
    std::string discriminator;
    /// the user's avatar hash
    std::string avatar;
    /// true, if the user is a bot
    bool bot = false;
    /// true, if the user has enabled two factor authentification
    bool mfa_enabled = false;
    /// the user's chosen language option
    std::string locale;
    /// true, if the user's email has been verified
    bool verified = false;
    /// the user's email
    std::string email;
    /// the flags on a user's account
    int flags = 0;
    /// the type of Nitro subscription on a user's account
    int premium_type = PremiumTypes::None;

   public:
    DLL_EXPORT User() = default;
    DLL_EXPORT User(const json& data, const std::string& token);
    DLL_EXPORT User(const std::string& id, const std::string& token);
    DLL_EXPORT virtual ~User() = default;

    /// get/create the DMChannel for this user
    DLL_EXPORT DMChannel get_dmchannel();
    /// send a message to this user
    DLL_EXPORT Message send(const std::string& content, const bool tts = false);
    /// send an embed to this user
    DLL_EXPORT Message send(const Embed& embed);

    ///@return Username as std::string
    DLL_EXPORT virtual explicit operator std::string() { return username; };

    /// the user's name
    DLL_EXPORT std::string get_username() { return username; }
    /// the user's discord-tag
    DLL_EXPORT std::string get_discriminator() { return discriminator; }
    /// the user's avatar hash
    DLL_EXPORT std::string get_avatar() { return avatar; }
    /// true, if the user is a bot
    DLL_EXPORT bool is_bot() { return bot; }
    /// true, if the user has enabled two factor authentification
    DLL_EXPORT bool is_mfa_enabled() { return mfa_enabled; }
    /// the user's chosen language option
    DLL_EXPORT std::string get_locale() { return locale; }
    /// true, if the user's email has been verified
    DLL_EXPORT bool is_verified() { return verified; }
    /// the user's email
    DLL_EXPORT std::string get_email() { return email; }
    /// the flags on a user's account
    DLL_EXPORT int get_flags() { return flags; }
    ///@return the type of Nitro subscription on a user's account
    DLL_EXPORT PremiumTypes get_premium_type() { return PremiumTypes(premium_type); }
};

}  // namespace DiscordCPP
