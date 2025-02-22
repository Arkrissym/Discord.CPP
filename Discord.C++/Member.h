#pragma once

#include "User.h"
#include "static.h"

namespace DiscordCPP {

class Member : public User {
   private:
    /// the member's nickname
    std::string nick;
    /// the member's roles
    std::vector<std::string> role_ids;
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

    /// @return the member's nickname
    DLL_EXPORT std::string get_nick() const { return nick; }
    /// @return the member's roles
    DLL_EXPORT std::vector<std::string> get_roles() const { return role_ids; }
    /// @return when the member joined the server
    DLL_EXPORT std::string get_joined_at() const { return joined_at; }
    /// @return true, if the member is defeaned
    DLL_EXPORT bool is_deaf() const { return deaf; }
    /// @return true, if the member is muted
    DLL_EXPORT bool is_mute() const { return mute; }
};

}  // namespace DiscordCPP
