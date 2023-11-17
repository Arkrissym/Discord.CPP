#include "Activity.h"

#include <utility>

DiscordCPP::Activity::Activity(std::string name, Type type, std::string url)
    : name(std::move(name)),
      type(type),
      url(std::move(url)) {
}

json DiscordCPP::Activity::to_json() {
    json json = {
        {"name", name},
        {"type", type}};

    if (type == Streaming)
        json["url"] = url;

    return json;
}
