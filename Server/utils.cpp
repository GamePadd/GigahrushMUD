#include "utils.h"

namespace Gigahrush {
	std::string toJson(const std::string& str, const std::string& type) {
		nlohmann::json resjs = { { "type", type }, { "content", str } };
		return resjs.dump();
	}
}