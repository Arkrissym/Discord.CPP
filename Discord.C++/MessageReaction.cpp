#include "MessageReaction.h"

DiscordCPP::MessageReaction::CountDetails::CountDetails(const json& data) {
    normal = get_or_else(data, "normal", 0);
    burst = get_or_else(data, "burst", 0);
}

DiscordCPP::MessageReaction::MessageReaction(const json& data, const std::string& token)
    : count_details(data.at("count_details")) {
    count = get_or_else(data, "count", 0);
    me = get_or_else(data, "me", false);
    me_burst = get_or_else(data, "me_burst", false);
    emoji = Emoji(data.at("emoji"), token);

    if (has_value(data, "burst_colors")) {
        for (json color : data.at("burst_colors")) {
            burst_colors.push_back(color.get<std::string>());
        }
    }
}
