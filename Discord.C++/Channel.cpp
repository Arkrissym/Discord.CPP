#include "Channel.h"

#include "Discord.h"
#include "static.h"

/**	@param[in]	data	JSON data
	@param[in]	token	discord token
*/
DiscordCPP::Channel::Channel(const json& data, const std::string& token) : DiscordCPP::DiscordObject(token) {
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
DiscordCPP::Channel::Channel(const std::string& id, const std::string& token) : DiscordCPP::DiscordObject(token) {
    std::string url = "/channels/" + id;

    *this = Channel(api_call(url), token);
}

/*	@param[in]	old	the Channel to copy
*/
DiscordCPP::Channel::Channel(const Channel& old) : DiscordCPP::DiscordObject(old) {
    id = old.id;
    type = old.type;
    position = old.position;
    //permission_overwrites
    name = old.name;
    icon = old.icon;
}

DiscordCPP::Channel* DiscordCPP::Channel::from_json(Discord* client, const json& data, const std::string& token) {
    Channel* channel;

    switch (data.at("type").get<int>()) {
        case ChannelType::GUILD_TEXT:
        case ChannelType::GUILD_NEWS:
            channel = (Channel*)new GuildChannel(data, token);
            break;
        case ChannelType::GUILD_VOICE:
            channel = (Channel*)new VoiceChannel(client, data, token);
            break;
        case ChannelType::DM:
        case ChannelType::GROUP_DM:
            channel = new DMChannel(data, token);
            break;
        default:
            channel = new Channel(data, token);
    }

    return channel;
}

void DiscordCPP::Channel::delete_channel() {
    std::string url = "/channels/" + id;

    api_call(url, "DEL");
}
