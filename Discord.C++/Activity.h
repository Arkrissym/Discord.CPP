#pragma once
#include <string>

#include "static.h"

namespace DiscordCPP {

// move this to Presence.h when implemented?
namespace DiscordStatus {
const std::string Online = "online";
const std::string DoNotDisturb = "dnd";
const std::string Idle = "idle";
const std::string Invisible = "invisible";
}  // namespace DiscordStatus

class Activity {
   public:
    enum Type {
        Game,
        Streaming,
        Listening,
        NoActivity
    };

   private:
    std::string name;
    Type type{Type::NoActivity};
    std::string url;

   public:
    DLL_EXPORT Activity() = default;
    DLL_EXPORT Activity(std::string name, Type type, std::string url = "");

    DLL_EXPORT json to_json();

    /// @return activity name
    std::string get_name() { return name; }
    /// @return activity type
    Type get_type() { return type; }
    /// @return activity url
    std::string get_url() { return url; }
};

}  // namespace DiscordCPP
