#pragma once
#include <iostream>

#include "User.h"
#include "static.h"

namespace DiscordCPP {

class Member : public User {
   public:
    ///the member's nickname
    std::string nick;
    //vector<Role> roles;
    ///when the member joined the server
    std::string joined_at;  //ISO8601 timestamp
    ///true, if the member is defeaned
    bool deaf = false;
    ///true, if the member is muted
    bool mute = false;

    DLL_EXPORT Member(const json& data, const std::string& token);
    DLL_EXPORT Member(){};

    ///@return Nickname (Username if not present) as std::string
    DLL_EXPORT operator std::string();
};

}  // namespace DiscordCPP
