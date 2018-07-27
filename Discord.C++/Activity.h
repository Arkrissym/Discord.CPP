#pragma once
#include <string>

#include <cpprest\json.h>

namespace DiscordCPP {

	using namespace std;
	using namespace web::json;

	//move this to Presence.h if existing
	namespace DiscordStatus {
		const string Online = "online";
		const string DoNotDisturb = "dnd";
		const string Idle = "idle";
		const string Invisible = "invisible";
	}

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

		__declspec(dllexport) Activity();
		__declspec(dllexport) Activity(string name, int type, string url="");
		__declspec(dllexport) ~Activity();

		__declspec(dllexport) value to_json();
	};

}
