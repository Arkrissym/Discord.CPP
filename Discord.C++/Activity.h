#pragma once
#include <string>

#include "static.h"

namespace DiscordCPP {

//move this to Presence.h when implemented?
namespace DiscordStatus {
const std::string Online = "online";
const std::string DoNotDisturb = "dnd";
const std::string Idle = "idle";
const std::string Invisible = "invisible";
}  // namespace DiscordStatus

namespace ActivityTypes {
enum ActivityTypes {
    Game,
    Streaming,
    Listening,
    NoActivity
};
}

class Activity {
   public:
    std::string name;
    int type;
    std::string url;

    DLL_EXPORT Activity();
    DLL_EXPORT Activity(const std::string& name, const int type, const std::string& url = "");

    DLL_EXPORT json to_json();
};

}  // namespace DiscordCPP
