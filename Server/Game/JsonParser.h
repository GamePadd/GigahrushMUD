#include <string>
#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"

namespace Gigahrush {
	class JSONParser {
	private:
		JSONParser() {}
		~JSONParser() {}

		JSONParser(JSONParser const&) = delete;
		JSONParser& operator= (JSONParser const&) = delete;
	public:
		static JSONParser& Instance();
		nlohmann::json readFile(std::string);
	};
}