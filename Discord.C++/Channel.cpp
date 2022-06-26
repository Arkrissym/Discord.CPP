#include "Channel.h"

#include "Discord.h"
#include "static.h"

/**	@param[in]	data	JSON data
    @param[in]	token	discord token
*/
DiscordCPP::Channel::Channel(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token) {
    data["id"].get_to<std::string>(id);
    data["type"].get_to<int>(type);
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

/*	@param[in]	old	the Channel to copy
 */
DiscordCPP::Channel::Channel(const Channel& old)
    : DiscordCPP::DiscordObject(old) {
    id = old.id;
    type = old.type;
    position = old.position;
    // permission_overwrites
    name = old.name;
    icon = old.icon;
}

DiscordCPP::Channel* DiscordCPP::Channel::from_json(Discord* client, const json& data, const std::string& token) {
    switch (data.at("type").get<int>()) {
        case ChannelType::GUILD_TEXT:
        case ChannelType::GUILD_NEWS:
            return (Channel*)new GuildChannel(data, token);
        case ChannelType::GUILD_VOICE:
            return (Channel*)new VoiceChannel(client, data, token);
        case ChannelType::DM:
        case ChannelType::GROUP_DM:
            return new DMChannel(data, token);
        default:
            return new Channel(data, token);
    }
}

void DiscordCPP::Channel::delete_channel() {
    std::string url = "/channels/" + id;

    api_call(url, "DEL");
}

DiscordCPP::Channel* DiscordCPP::Channel::copy() {
    switch (type) {
        case ChannelType::GUILD_TEXT:
        case ChannelType::GUILD_NEWS:
            return (Channel*)new GuildChannel(*(GuildChannel*)this);
        case ChannelType::GUILD_VOICE:
            return (Channel*)new VoiceChannel(*(VoiceChannel*)this);
        case ChannelType::DM:
        case ChannelType::GROUP_DM:
            return new DMChannel(*(DMChannel*)this);
        default:
            return new Channel(*this);
    }
}
