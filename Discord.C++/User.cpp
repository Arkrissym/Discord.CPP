#include "User.h"

#include "DMChannel.h"
#include "Embed.h"
#include "Message.h"
#include "cpprest/http_client.h"
#include "static.h"

using namespace std;
using namespace web::json;
using namespace utility;
using namespace web::http;
using namespace web::http::client;

DiscordCPP::User::User(const value& data, const string_t& token) : DiscordCPP::DiscordObject(token) {
    if (is_valid_field("id"))
        id = conversions::to_utf8string(data.at(U("id")).as_string());

    if (is_valid_field("username"))
        username = conversions::to_utf8string(data.at(U("username")).as_string());

    if (is_valid_field("discrimminator"))
        discriminator = conversions::to_utf8string(data.at(U("discriminator")).as_string());

    if (is_valid_field("avatar"))
        avatar = conversions::to_utf8string(data.at(U("avatar")).as_string());

    if (is_valid_field("bot"))
        bot = data.at(U("bot")).as_bool();

    if (is_valid_field("locale"))
        locale = conversions::to_utf8string(data.at(U("locale")).as_string());

    if (is_valid_field("mfa_enabled"))
        mfa_enabled = data.at(U("mfa_enabled")).as_bool();

    if (is_valid_field("verified"))
        verified = data.at(U("verified")).as_bool();

    if (is_valid_field("email"))
        email = conversions::to_utf8string(data.at(U("email")).as_string());

    if (is_valid_field("flags"))
        flags = data.at(U("flags")).as_integer();

    if (is_valid_field("premium_type"))
        premium_type = data.at(U("premium_type")).as_integer();
}

DiscordCPP::User::User(const string& id, const string_t& token) : DiscordCPP::DiscordObject(token) {
    string url = "/users/" + id;

    *this = User(api_call(url), token);
}

/** @return DMChannel
*/
DiscordCPP::DMChannel DiscordCPP::User::get_dmchannel() {
    value data;
    data[U("recipient_id")] = value(conversions::to_string_t(id));

    return DMChannel(api_call("/users/@me/channels", methods::POST, data), _token);
}

/**	@param[in]	content	The string message to send.
@param[in]	tts		(optional) Wether to send as tts-message or not. Default is false.
@return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::User::send(const string& content, const bool tts) {
    return Message(get_dmchannel().send(content, tts));
}

/**	@param[in]	embed	The Embed to send.
@return	The message that was sent.
*/
DiscordCPP::Message DiscordCPP::User::send(const DiscordCPP::Embed& embed) {
    return Message(get_dmchannel().send(embed));
}
