#pragma once

void addLog(std::vector<ftxui::Element>& logs, const nlohmann::json& obj) {
	size_t c = 1;

	logs.push_back(ftxui::text(""));
	logs.push_back(ftxui::text("---------------------"));
	logs.push_back(ftxui::text(""));

	if (obj["content"]["type"] == "Look") {
		//Базовая инфа

		logs.push_back(ftxui::hflow({
			ftxui::paragraph(obj["content"]["locationName"].get<std::string>()) | ftxui::color(DECORATE_COLOR), 
			ftxui::text(": Этаж ") | ftxui::color(DECORATE_COLOR),
			ftxui::paragraph(std::to_string(obj["content"]["coordinates"]["floor"].get<int>())) | ftxui::color(DECORATE_COLOR),
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50)) ;

		logs.push_back(ftxui::hflow({
			ftxui::paragraph("Координаты: ["), ftxui::paragraph(std::to_string(obj["content"]["coordinates"]["x"].get<int>())), ftxui::paragraph(", "),
			ftxui::paragraph(std::to_string(obj["content"]["coordinates"]["y"].get<int>())), ftxui::paragraph("]"),
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		//Пустая затычка v
		logs.push_back(ftxui::text(""));
		//Пустая затычка ^

		logs.push_back(ftxui::hflow({
			ftxui::paragraph("=== ОПИСАНИЕ ===") | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		logs.push_back(ftxui::text(""));

		logs.push_back(ftxui::hflow({
			ftxui::paragraph(obj["content"]["locationDescription"].get<std::string>()) | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		//Предметы и враги

		std::vector<nlohmann::json> items = obj["content"]["items"].get<nlohmann::json>();

		if (!items.empty()) {
			std::vector<ftxui::Element> itms;

			logs.push_back(ftxui::text(""));

			logs.push_back(ftxui::hflow({
				ftxui::paragraph("=== ПРЕДМЕТЫ ===") | ftxui::color(DECORATE_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

			logs.push_back(ftxui::text(""));

			c = 1;

			for (auto it : items) {
				itms.push_back(ftxui::paragraph(std::to_string(c) + ". " + it["description"].get<std::string>()) | ftxui::color(ITEM_COLOR));
				++c;
			}

			logs.push_back(ftxui::vbox(itms) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		std::vector<nlohmann::json> enemies = obj["content"]["enemies"].get<nlohmann::json>();

		if (!enemies.empty()) {
			std::vector<ftxui::Element> enms;

			logs.push_back(ftxui::text(""));

			logs.push_back(ftxui::hflow({
				ftxui::paragraph("=== ВРАГИ ===") | ftxui::color(DECORATE_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

			logs.push_back(ftxui::text(""));

			c = 1;

			for (auto it : enemies) {
				enms.push_back(ftxui::paragraph(std::to_string(c) + ". " + it["description"].get<std::string>()) | ftxui::color(ENEMY_COLOR));
				++c;
			}

			logs.push_back(ftxui::vbox(enms) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		//Направления

		std::vector<std::string> sides = obj["content"]["sides"].get<std::vector<std::string>>();
		std::string sidesAll;

		for (auto it : sides) {
			sidesAll += it + " ";
		}

		if (!sides.empty()) {
			logs.push_back(ftxui::text(""));

			logs.push_back(ftxui::hflow({
				ftxui::text("Вы можете пойти на ") | ftxui::color(DECORATE_COLOR),
				ftxui::paragraph(sidesAll) | ftxui::color(ITEM_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		//Этажи

		if (obj["content"]["coordinates"]["canGoUp"].get<bool>() == true) {
			logs.push_back(ftxui::text(""));

			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Из этой локации вы можете подняться на этаж выше") | ftxui::color(DECORATE_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		if (obj["content"]["coordinates"]["canGoDown"].get<bool>() == true) {
			logs.push_back(ftxui::text(""));

			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Из этой локации вы можете опуститься на этаж ниже") | ftxui::color(DECORATE_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		if (obj["content"]["coordinates"]["isExitBroken"].get<bool>() == true) {
			logs.push_back(ftxui::text(""));

			logs.push_back(ftxui::hflow({
				ftxui::paragraph("На локации сломан лифт, вы не можете перейти на другой этаж пока не почините лифт") | ftxui::color(DECORATE_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
	}
	else if (obj["content"]["type"] == "Move") {
		logs.push_back(ftxui::hflow({
			ftxui::paragraph(obj["content"]["res"].get<std::string>()) | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		nlohmann::json lookObj = obj["content"]["look"].get<nlohmann::json>();
		if (!lookObj.empty()) { addLog(logs,lookObj); }
	}
}