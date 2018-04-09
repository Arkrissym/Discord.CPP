#pragma once
#include <cpprest\json.h>

namespace DiscordCPP {

	using namespace web::json;

	class snowflake {
	protected:

	public:
		snowflake(uint64_t data);
	};

}