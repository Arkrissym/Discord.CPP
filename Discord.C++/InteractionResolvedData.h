#pragma once

#include <map>
#include "Channel.h"
#include "Member.h"
#include "Message.h"

namespace DiscordCPP {
class InteractionResolvedData {
   public:
    /// Map of user ids and user objects.
    std::map<std::string, User> users;

    /// Map of user ids and member objects.
    std::map<std::string, Member> members;

    // roles

    /// Map of channel ids and channel objects.
    std::map<std::string, Channel*> channels;

    /// Map of message ids and message objects.
    std::map<std::string, Message> messages;

    // attachments

    DLL_EXPORT InteractionResolvedData(const json& data, const std::string& token);
    DLL_EXPORT InteractionResolvedData(const InteractionResolvedData& other);
    DLL_EXPORT ~InteractionResolvedData();
};

}  // namespace DiscordCPP
