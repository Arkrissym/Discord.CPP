#pragma once
#include <iostream>

#include "User.h"
#include "static.h"

namespace DiscordCPP {

class Member : public User {
   private:
    /// the member's nickname
    std::string nick;
    // vector<Role> roles;
    /// when the member joined the server
    std::string joined_at;  // ISO8601 timestamp
    /// true, if the member is defeaned
    bool deaf = false;
    /// true, if the member is muted
    bool mute = false;

   public:
    DLL_EXPORT Member(const json& data, const std::string& token);
    DLL_EXPORT Member() = default;

    ///@return Nickname (Username if not present) as std::string
    DLL_EXPORT explicit operator std::string() override;

    /// the member's nickname
    DLL_EXPORT std::string get_nick() { return nick; }
    /// when the member joined the server
    DLL_EXPORT std::string get_joined_at() { return joined_at; }
    /// true, if the member is defeaned
    DLL_EXPORT bool is_deaf() { return deaf; }
    /// true, if the member is muted
    DLL_EXPORT bool is_mute() { return mute; }
};

}  // namespace DiscordCPP
