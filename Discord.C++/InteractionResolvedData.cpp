#include "InteractionResolvedData.h"

DiscordCPP::InteractionResolvedData::InteractionResolvedData(const json& data, const std::string& token) {
    if (has_value(data, "users")) {
        for (auto [key, value] : data["users"].items()) {
            users.insert(std::make_pair(key, User(value, token)));
        }
    }

    if (has_value(data, "members")) {
        for (auto [key, value] : data["members"].items()) {
            members.insert(std::make_pair(key, Member(value, token)));
        }
    }

    // roles

    if (has_value(data, "channels")) {
        for (auto [key, value] : data["channels"].items()) {
            channels.insert(std::make_pair(key, ChannelHelper::channel_from_json(nullptr, value, token)));
        }
    }

    if (has_value(data, "messages")) {
        for (auto [key, value] : data["messages"].items()) {
            Logger("InteractionResolvedData").debug(key + " : " + value.dump());
            messages.insert(std::make_pair(key, Message(value, token)));
        }
    }

    // attachments
}
