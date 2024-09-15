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
    DLL_EXPORT static Emoji by_name(const std::string& name);

    /// @return url encoded string in the format name:id
    DLL_EXPORT std::string url_encode();

    /// @return emoji id
    DLL_EXPORT std::optional<std::string> get_id();
    /// @return emoji name
    DLL_EXPORT std::optional<std::string> get_name() { return name; }
    /// @return roles allowed to use this emoji
    DLL_EXPORT std::vector<std::string> get_role_ids() { return role_ids; }
    /// @return user that created this emoji
    DLL_EXPORT std::optional<User> get_user() { return user; }
    /// @return wether this emoji must be wrapped in colons
    DLL_EXPORT bool requires_colons() { return require_colons; }
    /// @return wether this emoji is managed
    DLL_EXPORT bool is_managed() { return managed; }
    /// @return wether this emoji is animated
    DLL_EXPORT bool is_animated() { return animated; }
    /// @return whether this emoji can be used, may be false due to loss of Server Boosts
    DLL_EXPORT bool is_available() { return available; }
};
}  // namespace DiscordCPP
