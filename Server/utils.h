#pragma once

#include <string>
#include "nlohmann/json.hpp"

namespace Gigahrush {
	std::string toJson(const std::string&, const std::string&);
}