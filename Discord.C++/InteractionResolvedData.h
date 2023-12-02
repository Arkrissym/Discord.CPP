#pragma once

#include <map>

#include "Channel.h"
#include "ChannelHelper.h"
#include "Member.h"
#include "Message.h"

namespace DiscordCPP {
class InteractionResolvedData {
   private:
    /// Map of user ids and user objects.
    std::map<std::string, User> users;

    /// Map of user ids and member objects.
    std::map<std::string, Member> members;

    // roles

    /// Map of channel ids and channel objects.
    std::map<std::string, ChannelVariant> channels;

    /// Map of message ids and message objects.
    std::map<std::string, Message> messages;

    // attachments

   public:
    DLL_EXPORT InteractionResolvedData(const json& data, const std::string& token);

    /// @return Map of user ids and user objects.
    DLL_EXPORT std::map<std::string, User> get_users() { return users; }

    /// @return Map of user ids and member objects.
    DLL_EXPORT std::map<std::string, Member> get_members() { return members; }

    /// @return Map of channel ids and channel objects.
    DLL_EXPORT std::map<std::string, ChannelVariant> get_channels() { return channels; }

    /// @return Map of message ids and message objects.
    DLL_EXPORT std::map<std::string, Message> get_messages() { return messages; }
};

}  // namespace DiscordCPP
