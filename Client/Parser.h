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
			ftxui::paragraph("Координаты: [") | ftxui::color(DECORATE_COLOR), 
			ftxui::paragraph(std::to_string(obj["content"]["coordinates"]["x"].get<int>())) | ftxui::color(DECORATE_COLOR), ftxui::paragraph(", ") | ftxui::color(DECORATE_COLOR),
			ftxui::paragraph(std::to_string(obj["content"]["coordinates"]["y"].get<int>())) | ftxui::color(DECORATE_COLOR), ftxui::paragraph("]") | ftxui::color(DECORATE_COLOR),
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
	else if (obj["content"]["type"] == "Pickup") {
		if (obj["content"]["canPickup"] == false) {
			logs.push_back(ftxui::hflow({
				ftxui::text("Вы не можете подбирать предметы пока в комнате есть враги!") | ftxui::color(ENEMY_COLOR)
			}));
		}
		else {
			if (obj["content"]["isInventoryFull"] == true) {
				logs.push_back(ftxui::hflow({
					ftxui::text("Ваш инвентарь полон!") | ftxui::color(ENEMY_COLOR)
				}));
			}
			else {
				if (obj["content"]["pickuped"].get<bool>() == true) {
					logs.push_back(ftxui::hflow({
						ftxui::text("Вы подобрали ") | ftxui::color(DECORATE_COLOR), ftxui::text(obj["content"]["item"].get<std::string>()) | ftxui::color(ITEM_COLOR)
					}));
				}
				else {
					logs.push_back(ftxui::hflow({
						ftxui::text("Этого предмета нет в комнате") | ftxui::color(ENEMY_COLOR)
					}));
				}
			}
		}
	}
	else if (obj["content"]["type"] == "Drop") {
		if (obj["content"]["itemFound"] == false) {
			logs.push_back(ftxui::hflow({
				ftxui::text("У вас нет этого предмета") | ftxui::color(ENEMY_COLOR)
			}));
		}
		else {
			logs.push_back(ftxui::hflow({
				ftxui::text("Вы выбросили ") | ftxui::color(DECORATE_COLOR), ftxui::text(obj["content"]["dropped"].get<std::string>()) | ftxui::color(ENEMY_COLOR)
			}));
		}
	}
	else if (obj["content"]["type"] == "Me") {

		logs.push_back(ftxui::hflow({
			ftxui::text("Имя: ") | ftxui::color(DECORATE_COLOR), ftxui::text(obj["content"]["username"].get<std::string>()) | ftxui::color(ITEM_COLOR)
		}));
		logs.push_back(ftxui::hflow({
			ftxui::text("Уровень: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["level"].get<int>())) | ftxui::color(ITEM_COLOR)
		}));
		logs.push_back(ftxui::hflow({
			ftxui::text("Текущий опыт: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["exp"].get<int>())) | ftxui::color(ITEM_COLOR)
		}));
		logs.push_back(ftxui::hflow({
			ftxui::text("Опыта до следующего уровня: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["expToLU"].get<int>())) | ftxui::color(ITEM_COLOR)
			}));
		logs.push_back(ftxui::hflow({
			ftxui::text("Вместимость инвентаря: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["maxInventory"].get<int>())) | ftxui::color(ITEM_COLOR)
			}));
		logs.push_back(ftxui::hflow({
			ftxui::text("Здоровье: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["health"].get<int>())) | ftxui::color(ITEM_COLOR)
		}));
		logs.push_back(ftxui::hflow({
			ftxui::text("Броня: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["armor"].get<int>())) | ftxui::color(ITEM_COLOR)
			}));
		logs.push_back(ftxui::hflow({
			ftxui::text("Навык владения оружием: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["weaponSkill"].get<int>())) | ftxui::color(ITEM_COLOR)
		}));

		if (obj["content"]["currentWeapon"] != "") {
			logs.push_back(ftxui::hflow({
				ftxui::text("Текущее оружие: ") | ftxui::color(DECORATE_COLOR), ftxui::text(obj["content"]["currentWeapon"].get<std::string>()) | ftxui::color(ITEM_COLOR)
			}));
		}
	}
	else if (obj["content"]["type"] == "Inventory") {
		logs.push_back(ftxui::hflow({
			ftxui::text("Инвентарь: ") | ftxui::color(DECORATE_COLOR)
		}));

		std::vector<std::string> invItems = obj["content"]["items"].get<std::vector<std::string>>();
		c = 1;
		for (auto it : invItems) {
			logs.push_back(ftxui::hflow({
				ftxui::text(std::to_string(c) + ". ") | ftxui::color(DECORATE_COLOR), ftxui::text(it) | ftxui::color(ITEM_COLOR)
			}));
			++c;
		}
	}
	else if (obj["content"]["type"] == "LookItem") {
		logs.push_back(ftxui::hflow({
			ftxui::paragraph(obj["content"]["res"].get<std::string>()) | ftxui::color(DECORATE_COLOR)
		}));
	}
	else if (obj["content"]["type"] == "UseItem") {
		logs.push_back(ftxui::hflow({
			ftxui::paragraph(obj["content"]["res"].get<std::string>()) | ftxui::color(DECORATE_COLOR)
		}));
	}
	else if (obj["content"]["type"] == "") {

	}
	else if (obj["content"]["type"] == "") {

	}
}