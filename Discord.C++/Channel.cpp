#include "Channel.h"

#include "Permissions.h"
#include "static.h"

/**	@param[in]	data	JSON data
    @param[in]	token	discord token
*/
DiscordCPP::Channel::Channel(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token, data["id"].get<std::string>()),
      type(static_cast<Type>(data["type"].get<int>())) {
    position = get_or_else<int>(data, "position", 0);
    name = get_or_else<std::string>(data, "name", "");
    icon = get_or_else<std::string>(data, "icon", "");

    if (has_value(data, "permission_overwrites")) {
        for (auto d : data.at("permission_overwrites")) {
            permission_overwrites.push_back(PermissionOverwrites(d));
        }
    }
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

DiscordCPP::Permissions DiscordCPP::Channel::merge_permission_overwrites(const Permissions& permissions, const std::string& id) {
    if (permissions.has_permission(Permissions::ADMINISTRATOR)) {
        return Permissions::All();
    }

    Permissions result;
    result.add(permissions);

    for (auto overwrite : permission_overwrites) {
        if (overwrite.get_id() == id) {
            result.add(overwrite.get_allowed_permissions());
            result.remove(overwrite.get_denied_permissions());
        }
    }

    return result;
}
