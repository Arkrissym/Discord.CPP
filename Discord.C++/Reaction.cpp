#include "Reaction.h"

DiscordCPP::Reaction::Reaction(const json& data, const std::string& token) : BaseDiscordObject(token) {
    user_id = data.at("user_id").get<std::string>();
    channel_id = data.at("channel_id").get<std::string>();
    message_id = data.at("message_id").get<std::string>();
    guild_id = get_optional<std::string>(data, "guild_id");

    if (has_value(data, "member")) {
        member = Member(data.at("member"), token);
    }

    emoji = Emoji(data.at("emoji"), get_token());
    message_author_id = get_optional<std::string>(data, "message_author_id");
    burst = get_or_else<bool>(data, "burst", false);

    if (has_value(data, "burst_colors")) {
        for (json color : data.at("burst_colors")) {
            burst_colors.push_back(color.get<std::string>());
        }
    }

    type = static_cast<Type>(data.at("type").get<int>());
}

DiscordCPP::User DiscordCPP::Reaction::get_user() {
    if (!user.has_value()) {
        user = User(user_id, get_token());
    }

    return user.value();
}

DiscordCPP::TextChannel DiscordCPP::Reaction::get_channel() {
    if (!channel.has_value()) {
        channel = TextChannel(channel_id, get_token());
    }

    return channel.value();
}

DiscordCPP::Message DiscordCPP::Reaction::get_message() {
    if (!message.has_value()) {
        message = Message(message_id, get_token());
    }

    return message.value();
}

std::optional<DiscordCPP::Guild> DiscordCPP::Reaction::get_guild() {
    if (!guild_id.has_value() && guild_id.has_value()) {
        guild = Guild(nullptr, guild_id.value(), get_token());
    }

    return guild;
}

std::optional<DiscordCPP::User> DiscordCPP::Reaction::get_message_author() {
    if (!message_author.has_value() && message_author_id.has_value()) {
        message_author = User(message_author_id.value(), get_token());
    }

    return message_author;
}
