#pragma once

void addLog(std::vector<ftxui::Element>& logs, const nlohmann::json& obj) {
	if (obj["type"] != "ANSWER") { return; }


}