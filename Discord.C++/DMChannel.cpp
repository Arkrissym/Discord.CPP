#include "DMChannel.h"

#include <optional>

#include "TextChannel.h"
#include "User.h"
#include "static.h"

DiscordCPP::DMChannel::DMChannel(const json& data, const std::string& token)
    : DiscordCPP::TextChannel(data, token) {
    for (json recipient : data.at("recipients")) {
        recipients.emplace_back(recipient, token);
    }
    owner_id = get_optional<std::string>(data, "owner_id");
    application_id = get_or_else<std::string>(data, "application_id", "");
}

DiscordCPP::DMChannel::DMChannel(const std::string& id, const std::string& token)
    : DiscordCPP::TextChannel(token) {
    std::string url = "/channels/" + id;
    *this = DMChannel(api_call(url), token);
}

std::optional<DiscordCPP::User> DiscordCPP::DMChannel::get_owner() {
    if (owner_id.has_value() && owner == nullptr) {
        owner = new User(owner_id.value(), get_token());
    }
    return owner != nullptr ? std::optional<DiscordCPP::User>{*owner} : std::nullopt;
}
