#pragma once

#include <optional>
#include <string>
#include <vector>

#include "DiscordObject.h"
#include "User.h"

namespace DiscordCPP {
class Emoji : public DiscordObject {
   private:
    /// emoji name
    std::optional<std::string> name;
    /// roles allowed to use this emoji
    std::vector<std::string> role_ids;
    /// user that created this emoji
    std::optional<User> user;
    /// wether this emoji must be wrapped in colons
    bool require_colons;
    /// wether this emoji is managed
    bool managed;
    /// wether this emoji is animated
    bool animated;
    /// whether this emoji can be used, may be false due to loss of Server Boosts
    bool available;

   public:
    DLL_EXPORT Emoji() = default;
    DLL_EXPORT Emoji(const json& data, const std::string& token);

    /// @return emoji id
    std::optional<std::string> get_id();
    /// @return emoji name
    std::optional<std::string> get_name() { return name; }
    /// @return roles allowed to use this emoji
    std::vector<std::string> get_role_ids() { return role_ids; }
    /// @return user that created this emoji
    std::optional<User> get_user() { return user; }
    /// @return wether this emoji must be wrapped in colons
    bool requires_colons() { return require_colons; }
    /// @return wether this emoji is managed
    bool is_managed() { return managed; }
    /// @return wether this emoji is animated
    bool is_animated() { return animated; }
    /// @return whether this emoji can be used, may be false due to loss of Server Boosts
    bool is_available() { return available; }
};
}  // namespace DiscordCPP
