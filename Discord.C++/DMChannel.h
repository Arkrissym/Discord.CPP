#pragma once
#include "TextChannel.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

class DMChannel : public TextChannel {
   public:
    ///the recipients if the channel is DM
    std::vector<User> recipients;
    ///the owner of this DM channel
    User* owner;
    ///the application id of the group DM creator if it was bot-created
    std::string application_id;

    DLL_EXPORT DMChannel(const json& data, const std::string& token);
    DLL_EXPORT DMChannel(const std::string& id, const std::string& token);
    DLL_EXPORT DMChannel(const DMChannel& old);
};

}  // namespace DiscordCPP
