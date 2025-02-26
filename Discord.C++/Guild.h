#pragma once
#include <optional>
#include <string>
#include <vector>

#include "Channel.h"
#include "ChannelHelper.h"
#include "DiscordObject.h"
#include "Emoji.h"
#include "GuildChannel.h"
#include "Member.h"
#include "Role.h"
#include "User.h"
#include "VoiceChannel.h"

namespace DiscordCPP {

namespace VerificationLevel {
enum VerificationLevel {
    NONE,
    LOW,
    MEDIUM,
    HIGH,
    VERY_HIGH
};
}

namespace DefaultMessageNotificationLevel {
enum DefaultMessageNotificationLevel {
    ALL_MESSAGES,
    ONLY_MENTIONS
};
}

namespace ExplicitContentFilterLevel {
enum ExplicitContentFilterLevel {
    DISABLED,
    MEMBERS_WITHOUT_ROLES,
    ALL_MEMBERS
};
}

namespace MFALevel {
enum MFALevel {
    NONE,
    ELEVATED
};
}

class Discord;

class Guild : public DiscordObject {
   private:
    Discord* client = nullptr;
    MutexHolder mutex_holder;

    /// the guild's name
    std::string name;
    /// the guild's icon hash
    std::string icon;
    /// the guild's splash hash
    std::string splash;
    /// the owner of the guild
    std::optional<std::string> owner_id;
    std::optional<User> owner;
    /// total permissions for the user(doesn't include channel overrides)
    int permissions = 0;
    /// the guild's voice redion
    std::string region;
    /// the guild's afk channel(voice)
    std::optional<std::string> afk_channel_id;
    std::optional<VoiceChannel> afk_channel;
    /// afk timeout in seconds
    int afk_timeout = 0;
    /// true, if the guild is embeddable
    bool embed_enabled = false;
    /// the embedded channel
    std::optional<std::string> embed_channel_id;
    std::optional<GuildChannel> embed_channel;
    /// the VerificationLevel required to join the guild
    int verification_level = 0;
    /// DefaultMessageNotificationLevel
    int default_message_notifications = 0;
    /// ExplicitContentFilterLevel
    int explicit_content_filter = 0;
    /// roles in the guild
    std::vector<Role> roles;
    /// custom guild emojis
    std::vector<Emoji> emojis;
    /// enabled guild features
    std::vector<std::string> features;
    /// MFALevel
    int mfa_level = 0;
    /// application id of the guild creator if the guild is bot-created
    std::string application_id;  // snowflake
    /// true if the server widget is enabled
    bool widget_enabled = false;
    /// the channel for the server widget
    std::optional<std::string> widget_channel_id;
    std::optional<GuildChannel> widget_channel;
    /// the channel to which system messages are sent
    std::optional<std::string> system_channel_id;
    std::optional<GuildChannel> system_channel;

    // GUILD_CREATE event only
    /// when the guild was created
    std::string joined_at;  // ISO8601 timestamp
    /// true if it is a large guild
    bool large = false;
    /// true if the guild is not available
    bool unavailable = false;
    /// number of members
    int member_count = 0;
    /// users in the guild
    std::vector<Member> members;
    /// channels of the guild
    std::vector<ChannelVariant> channels;
    // vector<Presence> presences;

    DLL_EXPORT void _add_channel(ChannelVariant channel);
    DLL_EXPORT void _update_channel(ChannelVariant channel);
    DLL_EXPORT void _remove_channel(const std::string& channel_id);
    DLL_EXPORT void _add_member(Member member);
    DLL_EXPORT void _update_member(Member member);
    DLL_EXPORT void _remove_member(const std::string& member_id);
    DLL_EXPORT void _update_emojis(std::vector<Emoji> emojis);
    DLL_EXPORT void _add_role(Role role);
    DLL_EXPORT void _update_role(Role role);
    DLL_EXPORT void _remove_role(const std::string& role_id);

    friend Discord;

   public:
    DLL_EXPORT Guild(Discord* client, const json& data, const std::string& token);
    DLL_EXPORT Guild(Discord* client, const std::string& id, const std::string& token);
    DLL_EXPORT Guild() = default;

    ///@return Guild name as std::string
    DLL_EXPORT explicit operator std::string() { return name; };

    /// leave this guild
    DLL_EXPORT void leave();
    /// delete this guild
    DLL_EXPORT void delete_guild();

    /// kicks the given User from this Guild
    DLL_EXPORT void kick(const User& user);
    /// bans the given User from this Guild
    DLL_EXPORT void ban(const User& user, const std::string& reason, const int delete_message_days = 0);
    /// unbans the given User from this Guild
    DLL_EXPORT void unban(const User& user);

    /** @return the members permissions in the guild
        @param[in]  member  the member whose permissions to check
        @param[in]  channel the channel to for permission overwrites
        @throws DiscordException if an unknown role is encountered */
    DLL_EXPORT Permissions get_member_permissions(const Member& member, const ChannelVariant& channel);

    /// @return the guild's name
    DLL_EXPORT std::string get_name() { return name; }
    /// @return the guild's icon hash
    DLL_EXPORT std::string get_icon() { return icon; }
    /// @return the guild's splash hash
    DLL_EXPORT std::string get_splash() { return splash; }
    /// @return the id of the owner of the guild
    DLL_EXPORT std::optional<std::string> get_owner_id() { return owner_id; }
    /// @return the owner of the guild
    DLL_EXPORT std::optional<User> get_owner();
    /// @return total permissions for the user(doesn't include channel overrides)
    DLL_EXPORT int get_permissions() { return permissions; }
    /// @return the guild's voice redion
    DLL_EXPORT std::string get_region() { return region; }
    /// @return the id of the guild's afk channel(voice)
    DLL_EXPORT std::optional<std::string> get_afk_channel_id() { return afk_channel_id; }
    /// @return the guild's afk channel(voice)
    DLL_EXPORT std::optional<VoiceChannel> get_afk_channel();
    /// @return afk timeout in seconds
    DLL_EXPORT int get_afk_timeout() { return afk_timeout; }
    /// @return true, if the guild is embeddable
    DLL_EXPORT bool is_embed_enabled() { return embed_enabled; }
    /// @return the id of the embedded channel
    DLL_EXPORT std::optional<std::string> get_embed_channel_id() { return embed_channel_id; }
    /// @return the embedded channel
    DLL_EXPORT std::optional<GuildChannel> get_embed_channel();
    /// @return the VerificationLevel required to join the guild
    DLL_EXPORT int get_verification_level() { return verification_level; }
    /// @return DefaultMessageNotificationLevel
    DLL_EXPORT int get_default_message_notifications() { return default_message_notifications; }
    /// @return ExplicitContentFilterLevel
    DLL_EXPORT int get_explicit_content_filter() { return explicit_content_filter; }
    /// @return roles in the guild
    DLL_EXPORT std::vector<Role> get_roles() { return roles; }
    /// @return custom guild emojis
    DLL_EXPORT std::vector<Emoji> get_emojis() { return emojis; }
    /// @return enabled guild features
    DLL_EXPORT std::vector<std::string> get_features() { return features; }
    /// @return MFALevel
    DLL_EXPORT int get_mfa_level() { return mfa_level; }
    /// @return application id of the guild creator if the guild is bot-created
    DLL_EXPORT std::string get_application_id() { return application_id; }
    /// @return true if the server widget is enabled
    DLL_EXPORT bool is_widget_enabled() { return widget_enabled; }
    /// @return the id of the channel for the server widget
    DLL_EXPORT std::optional<std::string> get_widget_channel_id() { return widget_channel_id; }
    /// @return the channel for the server widget
    DLL_EXPORT std::optional<GuildChannel> get_widget_channel();
    /// @return the id of the channel to which system messages are sent
    DLL_EXPORT std::optional<std::string> get_system_channel_id() { return system_channel_id; }
    /// @return the channel to which system messages are sent
    DLL_EXPORT std::optional<GuildChannel> get_system_channel();

    // GUILD_CREATE event only
    /// @return when the guild was created
    DLL_EXPORT std::string get_joined_at() { return joined_at; }
    /// @return true if it is a large guild
    DLL_EXPORT bool is_large() { return large; }
    /// @return true if the guild is not available
    DLL_EXPORT bool is_unavailable() { return unavailable; }
    /// @return number of members
    DLL_EXPORT int get_member_count() { return member_count; }
    /// @return users in the guild
    DLL_EXPORT std::vector<Member> get_members() { return members; }
    /// @return channels of the guild
    DLL_EXPORT std::vector<ChannelVariant> get_channels() { return channels; }
};

}  // namespace DiscordCPP
