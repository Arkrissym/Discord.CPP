#include "Channel.h"

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

void DiscordCPP::Channel::delete_channel() {
    std::string url = "/channels/" + get_id();

    api_call(url, "DELETE");
}
