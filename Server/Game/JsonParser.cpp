#include "JsonParser.h"

namespace Gigahrush {
	JSONParser& JSONParser::Instance() {
		static JSONParser j;
		return j;
	}

	nlohmann::json JSONParser::readFile(std::string fileName) {
		std::ifstream file(fileName, std::ios::in);
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();
		return j;
	}
}