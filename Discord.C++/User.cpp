#include "User.h"

#include "DMChannel.h"
#include "Embed.h"
#include "Message.h"

DiscordCPP::User::User(const json& data, const std::string& token) : DiscordCPP::DiscordObject(token, data.at("id").get<std::string>()) {
    data.at("username").get_to<std::string>(username);
    data.at("discriminator").get_to<std::string>(discriminator);
    avatar = get_or_else<std::string>(data, "avatar", "");
    bot = get_or_else<bool>(data, "bot", false);
    locale = get_or_else<std::string>(data, "locale", "");
    mfa_enabled = get_or_else<bool>(data, "mfa_enabled", false);
    verified = get_or_else<bool>(data, "verified", false);
    email = get_or_else<std::string>(data, "email", "");
    flags = get_or_else<int>(data, "flags", 0);
    premium_type = get_or_else(data, "premium_type", PremiumTypes::None);
}

DiscordCPP::User::User(const std::string& id, const std::string& token) : DiscordCPP::DiscordObject(token) {
    std::string url = "/users/" + id;

    *this = User(api_call(url), token);
}

/** @return DMChannel
 */
DiscordCPP::DMChannel DiscordCPP::User::get_dmchannel() {
    json data = {{"recipient_id", get_id()}};

    return DMChannel(api_call("/users/@me/channels", "POST", data), get_token());
}

/**	@param[in]	content	The string message to send.
@param[in]	tts		(optional) Wether to send as tts-message or not. Default is false.
@return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::User::send(const std::string& content, const bool tts) {
    return Message(get_dmchannel().send(content, tts));
}

/**	@param[in]	embed	The Embed to send.
@return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::User::send(const DiscordCPP::Embed& embed) {
    return get_dmchannel().send(embed);
}
