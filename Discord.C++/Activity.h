#pragma once
#include <cpprest/json.h>

#include <string>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {

using namespace std;
using namespace web::json;

//move this to Presence.h when implemented?
namespace DiscordStatus {
const string Online = "online";
const string DoNotDisturb = "dnd";
const string Idle = "idle";
const string Invisible = "invisible";
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
    string name;
    int type;
    string url;

    DLL_EXPORT Activity();
    DLL_EXPORT Activity(const string& name, const int type, const string& url = "");

    DLL_EXPORT value to_json();
};

}  // namespace DiscordCPP
