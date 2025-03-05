#pragma once

#include <cstdint>
#include <vector>

#include "static.h"

namespace DiscordCPP {
class Permissions {
   public:
    enum Permission {
        /// Allows creation of instant invites
        CREATE_INSTANT_INVITE = 1,
        /// Allows kicking members
        KICK_MEMBERS = 1,
        /// Allows banning members
        BAN_MEMBERS = 2,
        /// Allows all permissions and bypasses channel permission overwrites
        ADMINISTRATOR = 3,
        /// Allows management and editing of channels
        MANAGE_CHANNELS = 4,
        /// Allows management and editing of the guild
        MANAGE_GUILD = 5,
        /// Allows for the addition of reactions to messages
        ADD_REACTIONS = 6,
        /// Allows for viewing of audit logs
        VIEW_AUDIT_LOG = 7,
        /// Allows for using priority speaker in a voice channel
        PRIORITY_SPEAKER = 8,
        /// Allows the user to go live
        STREAM = 9,
        /// Allows guild members to view a channel, which includes reading messages in text channels and joining voice channels
        VIEW_CHANNEL = 10,
        /// Allows for sending messages in a channel and creating threads in a forum (does not allow sending messages in threads)
        SEND_MESSAGES = 11,
        /// Allows for sending of /tts messages
        SEND_TTS_MESSAGES = 12,
        /// Allows for deletion of other users messages
        MANAGE_MESSAGES = 13,
        /// Links sent by users with this permission will be auto-embedded
        EMBED_LINKS = 14,
        /// Allows for uploading images and files
        ATTACH_FILES = 15,
        /// Allows for reading of message history
        READ_MESSAGE_HISTORY = 16,
        /// Allows for using the @everyone tag to notify all users in a channel, and the @here tag to notify all online users in a channel
        MENTION_EVERYONE = 17,
        /// Allows the usage of custom emojis from other servers
        USE_EXTERNAL_EMOJIS = 18,
        /// Allows for viewing guild insights
        VIEW_GUILD_INSIGHTS = 19,
        /// Allows for joining of a voice channel
        CONNECT = 20,
        /// Allows for speaking in a voice channel
        SPEAK = 21,
        /// Allows for muting members in a voice channel
        MUTE_MEMBERS = 22,
        /// Allows for deafening of members in a voice channel
        DEAFEN_MEMBERS = 23,
        /// Allows for moving of members between voice channels
        MOVE_MEMBERS = 24,
        /// Allows for using voice-activity-detection in a voice channel
        USE_VAD = 25,
        /// Allows for modification of own nickname
        CHANGE_NICKNAME = 26,
        /// Allows for modification of other users nicknames
        MANAGE_NICKNAMES = 27,
        /// Allows management and editing of roles
        MANAGE_ROLES = 28,
        /// Allows management and editing of webhooks
        MANAGE_WEBHOOKS = 29,
        /// Allows for editing and deleting emojis, stickers, and soundboard sounds created by all users
        MANAGE_GUILD_EXPRESSIONS = 30,
        /// Allows members to use application commands, including slash commands and context menu commands
        USE_APPLICATION_COMMANDS = 31,
        /// Allows for requesting to speak in stage channels. (This permission is under active development and may be changed or removed.)
        REQUEST_TO_SPEAK = 32,
        /// Allows for editing and deleting scheduled events created by all users
        MANAGE_EVENTS = 33,
        /// Allows for deleting and archiving threads, and viewing all private threads
        MANAGE_THREADS = 34,
        /// Allows for creating public and announcement threads
        CREATE_PUBLIC_THREADS = 35,
        /// Allows for creating private threads
        CREATE_PRIVATE_THREADS = 36,
        /// Allows the usage of custom stickers from other servers
        USE_EXTERNAL_STICKERS = 37,
        /// Allows for sending messages in threads
        SEND_MESSAGES_IN_THREADS = 38,
        /// Allows for using Activities (applications with the EMBEDDED flag) in a voice channel
        USE_EMBEDDED_ACTIVITIES = 39,
        /// Allows for timing out users to prevent them from sending or reacting to messages in chat and threads, and from speaking in voice and stage channels
        MODERATE_MEMBERS = 40,
        /// Allows for viewing role subscription insights
        VIEW_CREATOR_MONETIZATION_ANALYTICS = 41,
        /// Allows for using soundboard in a voice channel
        USE_SOUNDBOARD = 42,
        /// Allows for creating emojis, stickers, and soundboard sounds, and editing and deleting those created by the current user
        CREATE_GUILD_EXPRESSIONS = 43,
        /// Allows for creating scheduled events, and editing and deleting those created by the current user
        CREATE_EVENTS = 44,
        /// Allows the usage of custom soundboard sounds from other servers
        USE_EXTERNAL_SOUNDS = 45,
        /// Allows sending voice messages
        SEND_VOICE_MESSAGES = 46,
        /// Allows sending polls
        SEND_POLLS = 49,
        /// Allows user-installed apps to send public responses. When disabled, users will still be allowed to use their apps but the responses will be ephemeral. This only applies to apps not also installed to the server
        USE_EXTERNAL_APPS = 50
    };

   private:
    uint64_t permissions;

   public:
    DLL_EXPORT static Permissions All();

    DLL_EXPORT Permissions() : permissions(0) {};
    DLL_EXPORT Permissions(const std::string& permissions);

    DLL_EXPORT operator std::string();
    DLL_EXPORT json to_json();

    /// add the given permission
    DLL_EXPORT void add(const Permission& permission);
    /// merge the given permission set
    DLL_EXPORT void add(const Permissions& permissions);
    /// remove the given permission
    DLL_EXPORT void remove(const Permission& permission);
    /// remove the given permission set
    DLL_EXPORT void remove(const Permissions& permissions);

    /// check if the given permission is set
    DLL_EXPORT bool has_permission(Permission permission) const;
    /// check if all given permissions is set
    DLL_EXPORT bool has_all_permissions(std::vector<Permission> permissions) const;
    /// check if at least one of the given permissions is set
    DLL_EXPORT bool has_any_permission(std::vector<Permission> permissions) const;
};

class PermissionOverwrites {
   public:
    enum Type {
        ROLE = 0,
        MEMBER = 1
    };

   private:
    /// role or user id
    std::string id;
    /// overwite type
    Type type;
    /// allowed permissions
    Permissions allow;
    /// denied permissions
    Permissions deny;

   public:
    DLL_EXPORT PermissionOverwrites(const json& data);

    /// @return role or user id
    DLL_EXPORT std::string get_id() const { return id; }
    /// @return overwite type
    DLL_EXPORT Type get_type() const { return type; }
    /// @return allowed permissions
    DLL_EXPORT Permissions get_allowed_permissions() const { return allow; }
    /// @return denied permissions
    DLL_EXPORT Permissions get_denied_permissions() const { return deny; }
};
}  // namespace DiscordCPP
