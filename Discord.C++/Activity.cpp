#include "Activity.h"

DiscordCPP::Activity::Activity() {
    type = ActivityTypes::NoActivity;
}

DiscordCPP::Activity::Activity(const std::string& name, const int type, const std::string& url) {
    this->name = name;
    this->type = type;
    this->url = url;
}

DiscordCPP::json DiscordCPP::Activity::to_json() {
    DiscordCPP::json json = {
        {"name", name},
        {"type", type}};

    if (type == ActivityTypes::Streaming)
        json["url"] = url;

    return json;
}
