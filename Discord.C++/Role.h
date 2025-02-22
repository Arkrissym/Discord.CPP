#pragma once

#include "DiscordObject.h"
#include "Permissions.h"

namespace DiscordCPP {
class Role : public DiscordObject {
   private:
    enum Flag {
        /// role can be selected by members in an onboarding prompt
        IN_PROMPT = 1 << 0
    };

   private:
    std::string guild_id;
    /// role name
    std::string name;
    /// integer representation of hexadecimal color code
    unsigned int color;
    /// if this role is pinned in the user listing
    bool hoist;
    /// role icon hash
    std::optional<std::string> icon;
    /// role unicode emoji
    std::optional<std::string> unicode_emoji;
    /// position of this role (roles with the same position are sorted by id)
    unsigned int position;
    /// permission bit set;
    Permissions permissions;
    /// whether this role is managed by an integration
    bool managed;
    /// whether this role is mentionable
    bool mentionable;
    /// role flags combined as a bitfield
    int flags;

   public:
    DLL_EXPORT Role() = default;
    DLL_EXPORT Role(const json& data, const std::string& guild_id, const std::string& token);
    DLL_EXPORT Role(const std::string& id, const std::string& guild_id, const std::string& token);

    /**	delete this role
     *	@throws HTTPError
     */
    DLL_EXPORT void delete_role();

    /// @return role name
    DLL_EXPORT std::string get_name() const { return name; }
    /// @return integer representation of hexadecimal color code
    DLL_EXPORT unsigned int get_color() const { return color; }
    /// @return if this role is pinned in the user listing
    DLL_EXPORT bool is_hoist() const { return hoist; }
    /// @return role icon hash
    DLL_EXPORT std::optional<std::string> get_icon() const { return icon; }
    /// @return role unicode emoji
    DLL_EXPORT std::optional<std::string> get_unicode_emoji() const { return unicode_emoji; }
    /// @return position of this role (roles with the same position are sorted by id)
    DLL_EXPORT unsigned int get_position() const { return position; }
    /// @return permission bit set;
    DLL_EXPORT Permissions get_permissions() const { return permissions; }
    /// @return whether this role is managed by an integration
    DLL_EXPORT bool is_managed() const { return managed; }
    /// @return whether this role is mentionable
    DLL_EXPORT bool is_mentionable() const { return mentionable; }
    /// @return role flags combined as a bitfield
    DLL_EXPORT int get_flags() const { return flags; }
    /** @param[in] flag the Flag to check
     *	@return true if the role has the given flag
     */
    DLL_EXPORT bool has_flag(Flag flag) const { return flags & flag; }
};
}  // namespace DiscordCPP
