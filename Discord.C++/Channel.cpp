#include "Channel.h"

#include "Discord.h"
#include "static.h"

/**	@param[in]	data	JSON data
    @param[in]	token	discord token
*/
DiscordCPP::Channel::Channel(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token, data["id"].get<std::string>()),
      type(static_cast<Type>(data["type"].get<int>())) {
    position = get_or_else<int>(data, "position", 0);
    // permission_overwrites
    name = get_or_else<std::string>(data, "name", "");
    icon = get_or_else<std::string>(data, "icon", "");
}

/**	@param[in]	id		the channel's id
    @param[in]	token	discord token
*/
DiscordCPP::Channel::Channel(const std::string& id, const std::string& token)
    : DiscordCPP::DiscordObject(token) {
    std::string url = "/channels/" + id;

    *this = Channel(api_call(url), token);
}

/**	@param[in]	token	discord token
 */
DiscordCPP::Channel::Channel(const std::string& token)
    : DiscordCPP::DiscordObject(token) {
}

DiscordCPP::Channel* DiscordCPP::Channel::from_json(Discord* client, const json& data, const std::string& token) {
    switch (data.at("type").get<int>()) {
        case Type::GUILD_TEXT:
        case Type::GUILD_NEWS:
            return (Channel*)new GuildChannel(data, token);
        case Type::GUILD_VOICE:
            return (Channel*)new VoiceChannel(client, data, token);
        case Type::DM:
        case Type::GROUP_DM:
            return new DMChannel(data, token);
        default:
            return new Channel(data, token);
    }
}

void DiscordCPP::Channel::delete_channel() {
    std::string url = "/channels/" + get_id();

    api_call(url, "DELETE");
}

DiscordCPP::Channel* DiscordCPP::Channel::copy() {
    switch (type) {
        case Type::GUILD_TEXT:
        case Type::GUILD_NEWS:
            return (Channel*)new GuildChannel(*(GuildChannel*)this);
        case Type::GUILD_VOICE:
            return (Channel*)new VoiceChannel(*(VoiceChannel*)this);
        case Type::DM:
        case Type::GROUP_DM:
            return new DMChannel(*(DMChannel*)this);
        default:
            return new Channel(*this);
    }
}
