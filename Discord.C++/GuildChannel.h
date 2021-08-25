#pragma once
#include <cpprest/json.h>

#include "TextChannel.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

using namespace std;
using namespace web::json;
using namespace utility;

class Guild;

class GuildChannel : public TextChannel {
   public:
    ///the guild the channel belongs to
    Guild* guild = NULL;
    ///the parent category channel
    Channel* parent;

    DLL_EXPORT GuildChannel(const value& data, const string_t& token);
    DLL_EXPORT GuildChannel(const string& id, const string_t& token);
    DLL_EXPORT GuildChannel(const GuildChannel& old);
    DLL_EXPORT GuildChannel(){};
    DLL_EXPORT ~GuildChannel();
};

}  // namespace DiscordCPP
