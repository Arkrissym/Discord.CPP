#include "DMChannel.h"

#include "User.h"
#include "static.h"

DiscordCPP::DMChannel::DMChannel(const json& data, const std::string& token)
    : DiscordCPP::TextChannel(data, token) {
    for (json recipient : data.at("recipients")) {
        recipients.emplace_back(recipient, token);
    }
    owner_id = data.at("owner_id").get<std::string>();
    application_id = get_or_else<std::string>(data, "application_id", "");
}

DiscordCPP::DMChannel::DMChannel(const std::string& id, const std::string& token) {
    std::string url = "/channels/" + id;
    *this = DMChannel(api_call(url), token);
}

DiscordCPP::User DiscordCPP::DMChannel::get_owner() {
    if (owner == nullptr) {
        owner = new User(owner_id, get_token());
    }
    return *owner;
}
