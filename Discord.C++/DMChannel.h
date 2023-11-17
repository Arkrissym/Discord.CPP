#pragma once
#include "TextChannel.h"

namespace DiscordCPP {

class DMChannel : public TextChannel {
   private:
    /// the recipients if the channel is DM
    std::vector<User> recipients;
    /// the owner of this DM channel, only for group dm channels
    std::optional<std::string> owner_id;
    User* owner = nullptr;
    /// the application id of the group DM creator if it was bot-created
    std::string application_id;

   public:
    DLL_EXPORT DMChannel(const json& data, const std::string& token);
    DLL_EXPORT DMChannel(const std::string& id, const std::string& token);

    /// the recipients if the channel is DM
    DLL_EXPORT std::vector<User> get_recipients() { return recipients; }
    /// the owner of this DM channel
    DLL_EXPORT std::optional<User> get_owner();
    /// the application id of the group DM creator if it was bot-created
    DLL_EXPORT std::string get_application_id() { return application_id; }
};

}  // namespace DiscordCPP
