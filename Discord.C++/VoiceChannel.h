#pragma once
#include <cpprest/json.h>

#include "Channel.h"

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
class Discord;
class VoiceClient;

class VoiceChannel : public Channel {
   private:
    Discord* _client;

   public:
    ///the channel's bitrate
    int bitrate;
    ///the channel's user-limit
    int user_limit;
    ///the parent category channel
    Channel* parent = NULL;
    ///the guild the channel belongs to
    Guild* guild = NULL;

    DLL_EXPORT VoiceChannel(Discord* client, const value& data, const string_t& token);
    DLL_EXPORT VoiceChannel(Discord* client, const string& id, const string_t& token);
    DLL_EXPORT VoiceChannel(const VoiceChannel& old);
    DLL_EXPORT VoiceChannel(){};
    DLL_EXPORT ~VoiceChannel();

    ///connect to this VoiceChannel
    DLL_EXPORT VoiceClient* connect();
};

}  // namespace DiscordCPP
