#include "Interaction.h"

#include "Embed.h"

DiscordCPP::Interaction::Interaction(const json& data, const std::string& token)
    : DiscordCPP::DiscordObject(token, data.at("id").get<std::string>()),
      type(static_cast<Type>(data.at("type").get<int>())) {
    data.at("application_id").get_to<std::string>(application_id);

    if (has_value(data, "data")) {
        this->data.emplace(data.at("data"), token);
    }
    guild_id = get_optional<std::string>(data, "guild_id");
    channel_id = get_optional<std::string>(data, "channel_id");
    if (has_value(data, "member")) {
        member.emplace(data.at("member"), token);
    }
    if (has_value(data, "user")) {
        user.emplace(data.at("user"), token);
    }
    data.at("token").get_to<std::string>(this->token);
    if (has_value(data, "message")) {
        message.emplace(data.at("message"), token);
    }
    locale = get_optional<std::string>(data, "locale");
    guild_locale = get_optional<std::string>(data, "guild_locale");
}

void DiscordCPP::Interaction::reply(const std::string& content, const bool tts) {
    std::string url = "/interactions/" + get_id() + "/" + token + "/callback";

    json data = {
        {"type", 4},
        {
            "data",
            {
                //
                {"content", content},
                {"tts", tts}  //
            }                 //
        }                     //
    };

    api_call(url, "POST", data, "application/json");
}

void DiscordCPP::Interaction::reply(Embed embed) {
    std::string url = "/interactions/" + get_id() + "/" + token + "/callback";

    json data = {{"type", 4}};

    data["data"]["embeds"].push_back(embed.to_json());

    api_call(url, "POST", data, "application/json");
}

void DiscordCPP::Interaction::update_reply(const std::string& content, const bool tts) {
    std::string url = "/webhooks/" + application_id + "/" + token + "/messages/@original";

    json data = {
        {"content", content},
        {"tts", tts}  //
    };

    api_call(url, "PATCH", data, "application/json");
}

void DiscordCPP::Interaction::update_reply(Embed embed) {
    std::string url = "/webhooks/" + application_id + "/" + token + "/messages/@original";

    json data;

    data["embeds"].push_back(embed.to_json());

    api_call(url, "PATCH", data, "application/json");
}
