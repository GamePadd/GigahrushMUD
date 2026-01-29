#pragma once

void addEnemyAttack(std::vector<ftxui::Element>& logs, const nlohmann::json& enemyStep) {
	logs.push_back(ftxui::hflow({
		ftxui::text(enemyStep["enemyName"].get<std::string>()) | ftxui::color(ENEMY_COLOR),
		ftxui::text(" сказал: ") | ftxui::color(DECORATE_COLOR),
		ftxui::paragraph(enemyStep["replic"].get<std::string>()) | ftxui::color(ENEMY_COLOR)
	}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

	if (enemyStep["hasArmor"].get<bool>() == true) {
		logs.push_back(ftxui::hflow({
			ftxui::text(enemyStep["enemyName"].get<std::string>()) | ftxui::color(ENEMY_COLOR),
			ftxui::text(" ударил вас на ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(std::to_string(enemyStep["loseHealth"].get<int>())) | ftxui::color(ENEMY_COLOR),
			ftxui::text(" урона и снёс вам ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(std::to_string(enemyStep["loseArmor"].get<int>())) | ftxui::color(ENEMY_COLOR),
			ftxui::text(" единиц брони.") | ftxui::color(DECORATE_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Ваше здоровье: ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(std::to_string(enemyStep["currentHealth"].get<int>())) | ftxui::color(ITEM_COLOR),
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Ваша броня: ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(std::to_string(enemyStep["currentArmor"].get<int>())) | ftxui::color(ITEM_COLOR),
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
	else {
		logs.push_back(ftxui::hflow({
			ftxui::text(enemyStep["enemyName"].get<std::string>()) | ftxui::color(ENEMY_COLOR),
			ftxui::text(" ударил вас на ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(std::to_string(enemyStep["loseHealth"].get<int>())) | ftxui::color(ENEMY_COLOR),
			ftxui::text(" урона.") | ftxui::color(DECORATE_COLOR),
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Ваше здоровье: ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(std::to_string(enemyStep["currentHealth"].get<int>())) | ftxui::color(ITEM_COLOR),
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Ваша броня: ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(std::to_string(enemyStep["currentArmor"].get<int>())) | ftxui::color(ITEM_COLOR),
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
}

void addPlayerDeath(std::vector<ftxui::Element>& logs, const nlohmann::json& obj) {
	if (obj["isDead"].get<bool>() != true) { return; }

	logs.push_back(ftxui::hflow({
		ftxui::paragraph("Вы умерли, ваши вещи остались в комнате [") | ftxui::color(DECORATE_COLOR),
		ftxui::paragraph(std::to_string(obj["x"].get<int>())) | ftxui::color(DECORATE_COLOR), ftxui::paragraph(", ") | ftxui::color(DECORATE_COLOR),
		ftxui::paragraph(std::to_string(obj["y"].get<int>())) | ftxui::color(DECORATE_COLOR), ftxui::text("] на ") | ftxui::color(DECORATE_COLOR),
		ftxui::paragraph(std::to_string(obj["Floor"].get<int>())) | ftxui::color(DECORATE_COLOR),
		ftxui::text(" этаже") | ftxui::color(DECORATE_COLOR)
	}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50)) ;

	if (obj["canRespawn"].get<bool>() == false) {
		logs.push_back(ftxui::hflow({
			ftxui::paragraph("Вы не можете возродиться во время самосбора, после окончания самосбора вы возродитесь автоматически!") | ftxui::color(ENEMY_COLOR),
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
}

void addLevelUp(std::vector<ftxui::Element>& logs, const nlohmann::json& obj) {
	if (obj["isLevelUp"].get<bool>() == false) { return; }

	logs.push_back(ftxui::hflow({
		ftxui::text("Вы получили новый уровень: ") | ftxui::color(DECORATE_COLOR),
		ftxui::text(std::to_string(obj["newLevel"].get<int>())) | ftxui::color(ITEM_COLOR),
	}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

	logs.push_back(ftxui::hflow({
		ftxui::text("Опыта до следующего уровня: ") | ftxui::color(DECORATE_COLOR),
		ftxui::text(std::to_string(obj["nextLevelExp"].get<int>())) | ftxui::color(ITEM_COLOR),
	}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

	logs.push_back(ftxui::hflow({
		ftxui::text("Текущий опыт: ") | ftxui::color(DECORATE_COLOR),
		ftxui::text(std::to_string(obj["currentExp"].get<int>())) | ftxui::color(ITEM_COLOR),
	}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
}

void addLog(std::vector<ftxui::Element>& logs, const nlohmann::json& obj) {
	//Clear log if big
	
	if (logs.size() > MAX_LOGS) {
		while (logs.size() > MAX_LOGS) {
			//logs.pop_front();
			logs.erase(logs.begin());
		}
	}

	size_t c = 1;

	logs.push_back(ftxui::text(""));
	logs.push_back(ftxui::text("---------------------"));
	logs.push_back(ftxui::text(""));

	if (obj["content"]["type"] == "Look") {
		//Базовая инфа

		logs.push_back(ftxui::hflow({
			ftxui::paragraph(obj["content"]["locationName"].get<std::string>()) | ftxui::color(ITEM_COLOR), 
			ftxui::text(": Этаж ") | ftxui::color(DECORATE_COLOR),
			ftxui::paragraph(std::to_string(obj["content"]["coordinates"]["floor"].get<int>())) | ftxui::color(ITEM_COLOR),
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
			ftxui::paragraph(obj["content"]["locationDescription"].get<std::string>()) | ftxui::color(ADD_COLOR1)
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
				ftxui::paragraph("Из этой локации вы можете подняться на этаж выше") | ftxui::color(ITEM_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		if (obj["content"]["coordinates"]["canGoDown"].get<bool>() == true) {
			logs.push_back(ftxui::text(""));

			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Из этой локации вы можете опуститься на этаж ниже") | ftxui::color(ITEM_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		if (obj["content"]["coordinates"]["isExitBroken"].get<bool>() == true) {
			logs.push_back(ftxui::text(""));

			logs.push_back(ftxui::hflow({
				ftxui::paragraph("На локации сломан выход, вы не можете перейти на другой этаж пока не почините его") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		return;
	}
	else if (obj["content"]["type"] == "Move") {
		if (obj["content"]["canMove"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вы не можете пойти в эту сторону!") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["unknownSide"].get<bool>() == true) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Неизвестная сторона!") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}

		if (obj["content"]["moved"].get<bool>() == true) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вы переместились!") | ftxui::color(ITEM_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
		else {
			return;
		}

		nlohmann::json lookObj = obj["content"]["look"].get<nlohmann::json>();
		if (!lookObj.empty()) { addLog(logs,lookObj); }

		return;
	}
	else if (obj["content"]["type"] == "Pickup") {
		if (obj["content"]["canPickup"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вы не можете подбирать предметы пока в комнате есть враги!") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
		else {
			if (obj["content"]["isInventoryFull"].get<bool>() == true) {
				logs.push_back(ftxui::hflow({
					ftxui::paragraph("Ваш инвентарь полон!") | ftxui::color(ENEMY_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			}
			else {
				if (obj["content"]["pickuped"].get<bool>() == true) {
					logs.push_back(ftxui::hflow({
						ftxui::text("Вы подобрали ") | ftxui::color(DECORATE_COLOR), ftxui::text(obj["content"]["item"].get<std::string>()) | ftxui::color(ITEM_COLOR)
					}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
				}
				else {
					logs.push_back(ftxui::hflow({
						ftxui::paragraph("Этого предмета нет в комнате") | ftxui::color(ENEMY_COLOR)
					}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
				}
			}
		}
		return;
	}
	else if (obj["content"]["type"] == "Drop") {
		if (obj["content"]["itemFound"] == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("У вас нет этого предмета") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
		else {
			logs.push_back(ftxui::hflow({
				ftxui::text("Вы выбросили ") | ftxui::color(DECORATE_COLOR), ftxui::text(obj["content"]["dropped"].get<std::string>()) | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
		return;
	}
	else if (obj["content"]["type"] == "Me") {
		logs.push_back(ftxui::hflow({
			ftxui::text("Имя: ") | ftxui::color(DECORATE_COLOR), ftxui::text(obj["content"]["username"].get<std::string>()) | ftxui::color(ITEM_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Уровень: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["level"].get<int>())) | ftxui::color(ITEM_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Текущий опыт: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["exp"].get<int>())) | ftxui::color(ITEM_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Опыта до следующего уровня: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["expToLU"].get<int>())) | ftxui::color(ITEM_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Вместимость инвентаря: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["maxInventory"].get<int>())) | ftxui::color(ITEM_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Здоровье: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["health"].get<int>())) | ftxui::color(ITEM_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Броня: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["armor"].get<int>())) | ftxui::color(ITEM_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		logs.push_back(ftxui::hflow({
			ftxui::text("Навык владения оружием: ") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(obj["content"]["weaponSkill"].get<int>())) | ftxui::color(ITEM_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		if (obj["content"]["currentWeapon"] != "") {
			logs.push_back(ftxui::hflow({
				ftxui::text("Текущее оружие: ") | ftxui::color(DECORATE_COLOR), ftxui::text(obj["content"]["currentWeapon"].get<std::string>()) | ftxui::color(ITEM_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
		return;
	}
	else if (obj["content"]["type"] == "Inventory") {
		logs.push_back(ftxui::hflow({
			ftxui::text("Инвентарь: ") | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		std::vector<std::string> invItems = obj["content"]["items"].get<std::vector<std::string>>();
		c = 1;
		for (auto it : invItems) {
			logs.push_back(ftxui::hflow({
				ftxui::text(std::to_string(c) + ". ") | ftxui::color(DECORATE_COLOR), ftxui::text(it) | ftxui::color(ITEM_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			++c;
		}
		return;
	}
	else if (obj["content"]["type"] == "LookItem") {
		if(obj["content"]["found"].get<bool>() == true) {
			nlohmann::json obbj = obj["content"]["object"].get<nlohmann::json>();

			if (!obbj.empty()) {
				if (obbj["type"] == "Item") {
					logs.push_back(ftxui::hflow({
						ftxui::text("Предмет: ") | ftxui::color(DECORATE_COLOR),
						ftxui::text(obbj["name"].get<std::string>()) | ftxui::color(ITEM_COLOR)
					}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

					logs.push_back(ftxui::hflow({
						ftxui::text("Описание: ") | ftxui::color(DECORATE_COLOR),
						ftxui::paragraph(obbj["description"].get<std::string>()) | ftxui::color(ADD_COLOR1)
					}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

					if (obbj["ItemType"] == "Component") {
						logs.push_back(ftxui::hflow({
							ftxui::paragraph("Нельзя использовать") | ftxui::color(ENEMY_COLOR),
						}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

						return;
					}
					if (obbj["ItemType"] == "Weapon") {
						logs.push_back(ftxui::hflow({
							ftxui::text("Урон: ") | ftxui::color(DECORATE_COLOR),
							ftxui::text(std::to_string(obbj["damage"].get<int>())) | ftxui::color(ITEM_COLOR)
						}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

						logs.push_back(ftxui::hflow({
							ftxui::paragraph("Вы можете экипировать и атаковать этим предметом") | ftxui::color(ITEM_COLOR),
						}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

						return;
					}
					if (obbj["ItemType"] == "Armor") {
						logs.push_back(ftxui::hflow({
							ftxui::text("Броня: ") | ftxui::color(DECORATE_COLOR),
							ftxui::text(std::to_string(obbj["armor"].get<int>())) | ftxui::color(ITEM_COLOR)
							}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

						logs.push_back(ftxui::hflow({
							ftxui::paragraph("Вы можете использовать этот предмет") | ftxui::color(ITEM_COLOR),
						}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

						return;
					}
					if (obbj["ItemType"] == "Heal") {
						logs.push_back(ftxui::hflow({
							ftxui::text("Здоровье: ") | ftxui::color(DECORATE_COLOR),
							ftxui::text(std::to_string(obbj["heal"].get<int>())) | ftxui::color(ITEM_COLOR)
							}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

						logs.push_back(ftxui::hflow({
							ftxui::paragraph("Вы можете использовать этот предмет") | ftxui::color(ITEM_COLOR),
						}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

						return;
					}
				}
				if (obbj["type"] == "Enemy") {
					logs.push_back(ftxui::hflow({
						ftxui::text("Враг: ") | ftxui::color(DECORATE_COLOR),
						ftxui::text(obbj["name"].get<std::string>()) | ftxui::color(ITEM_COLOR)
					}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

					logs.push_back(ftxui::hflow({
						ftxui::text("Описание: ") | ftxui::color(DECORATE_COLOR),
						ftxui::paragraph(obbj["description"].get<std::string>()) | ftxui::color(ADD_COLOR1)
					}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

					logs.push_back(ftxui::hflow({
						ftxui::text("Здоровье: ") | ftxui::color(DECORATE_COLOR),
						ftxui::text(std::to_string(obbj["health"].get<int>())) | ftxui::color(ENEMY_COLOR)
					}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

					logs.push_back(ftxui::hflow({
						ftxui::text("Урон: ") | ftxui::color(DECORATE_COLOR),
						ftxui::text(std::to_string(obbj["damage"].get<int>())) | ftxui::color(ENEMY_COLOR)
					}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
				}
				return;
			}
		}
		else {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("У вас не такого предмета или такого врага нет в комнате!") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
		return;
	}
	else if (obj["content"]["type"] == "UseItem") {
		if (obj["content"]["haveItem"] == true) {
			nlohmann::json itmm = obj["content"]["item"].get<nlohmann::json>();
			if (!itmm.empty()) {
				if (itmm["type"] == "Component") {
					logs.push_back(ftxui::hflow({
						ftxui::paragraph("Вы не можете использовать компонент!") | ftxui::color(ENEMY_COLOR)
					}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
				}
				if (itmm["type"] == "Weapon") {
					logs.push_back(ftxui::hflow({
						ftxui::paragraph("Вы не можете использовать оружие, вы можете его только экипировать!") | ftxui::color(ENEMY_COLOR)
					}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
				}
				if (itmm["type"] == "Armor") {
					if (itmm["used"] == true) {
						logs.push_back(ftxui::hflow({
							ftxui::text("Вы использовали ") | ftxui::color(DECORATE_COLOR),
							ftxui::text(itmm["name"].get<std::string>()) | ftxui::color(DECORATE_COLOR)
						}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
						logs.push_back(ftxui::hflow({
							ftxui::paragraph(itmm["description"].get<std::string>()) | ftxui::color(ADD_COLOR1)
						}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
						logs.push_back(ftxui::hflow({
							ftxui::text("Вам добавлено ") | ftxui::color(DECORATE_COLOR),
							ftxui::text(std::to_string(itmm["armor"].get<int>())) | ftxui::color(ITEM_COLOR),
							ftxui::text(" единиц брони") | ftxui::color(DECORATE_COLOR)
						}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
					}
					else {
						logs.push_back(ftxui::hflow({
							ftxui::text("У вас уже полная броня!") | ftxui::color(ITEM_COLOR)
						}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
					}
				}
				if (itmm["type"] == "Heal") {
					if (itmm["used"] == true) {
						logs.push_back(ftxui::hflow({
							ftxui::text("Вы использовали ") | ftxui::color(DECORATE_COLOR),
							ftxui::text(itmm["name"].get<std::string>()) | ftxui::color(DECORATE_COLOR)
							}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
						logs.push_back(ftxui::hflow({
							ftxui::paragraph(itmm["description"].get<std::string>()) | ftxui::color(ADD_COLOR1)
							}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
						logs.push_back(ftxui::hflow({
							ftxui::text("Вам добавлено ") | ftxui::color(DECORATE_COLOR),
							ftxui::text(std::to_string(itmm["heal"].get<int>())) | ftxui::color(ITEM_COLOR),
							ftxui::text(" единиц здоровья") | ftxui::color(DECORATE_COLOR)
							}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
					}
					else {
						logs.push_back(ftxui::hflow({
							ftxui::text("У вас уже полное здоровье!") | ftxui::color(ITEM_COLOR)
						}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
					}
				}
			}
		}
		else {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("У вас нет этого предмета!") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		nlohmann::json enemyStep = obj["content"]["enemyStep"].get<nlohmann::json>();

		if (!enemyStep.empty()) {
			addEnemyAttack(logs, enemyStep);
		}

		nlohmann::json playerDeath = obj["content"]["checkPlayerDeath"].get<nlohmann::json>();

		if (!playerDeath.empty()) {
			addPlayerDeath(logs, playerDeath);
		}
		return;
	}
	else if (obj["content"]["type"] == "Craft") {
		if (obj["content"]["noItemFound"].get<bool>() == true) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Такой предмет не найден") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["noResources"].get<bool>() == true) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("У вас недостаточно ресурсов для крафта") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["noCraftFound"].get<bool>() == true) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("У этого предмета нет крафтов") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}

		std::vector<std::string> losted = obj["content"]["lostedItems"].get<std::vector<std::string>>();

		logs.push_back(ftxui::hflow({
			ftxui::text("Вы скрафтили предмет ") | ftxui::color(DECORATE_COLOR),
			ftxui::paragraph(obj["content"]["craftedItem"].get<std::string>()) | ftxui::color(ITEM_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		logs.push_back(ftxui::text(""));

		c = 1;

		logs.push_back(ftxui::hflow({
			ftxui::paragraph("Потерянные предметы: ") | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		for (auto it : losted) {
			logs.push_back(ftxui::hflow({
				ftxui::text(std::to_string(c) + ". ") | ftxui::color(DECORATE_COLOR), ftxui::text(it) | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			++c;
		}
		return;
	}
	else if (obj["content"]["type"] == "ChangeFloor") {
		if (obj["content"]["isPlayerBrokeElevator"].get<bool>() == true) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вы сломали выход, вы не сможете сменить этаж пока не почините его") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["isElevatorBroken"].get<bool>() == true) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вы не можете сменить этаж") | ftxui::color(ENEMY_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["canChange"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вы не можете поменять этаж в этой комнате") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["canGoUp"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вы не можете подняться выше") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["canGoDown"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вы не можете опуститься ниже") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["changed"].get<bool>() == true) {
			if (obj["content"]["moveType"].get<int>() == 1) {
				logs.push_back(ftxui::hflow({
					ftxui::text("Вы поднялись на ") | ftxui::color(DECORATE_COLOR),
					ftxui::paragraph(std::to_string(obj["content"]["changedFloor"].get<int>())) | ftxui::color(ITEM_COLOR),
					ftxui::text(" этаж") | ftxui::color(DECORATE_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			}
			else {
				logs.push_back(ftxui::hflow({
					ftxui::text("Вы опустились на ") | ftxui::color(DECORATE_COLOR),
					ftxui::paragraph(std::to_string(obj["content"]["changedFloor"].get<int>())) | ftxui::color(ITEM_COLOR),
					ftxui::text(" этаж") | ftxui::color(DECORATE_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			}
		}
		return;
	}
	else if (obj["content"]["type"] == "Recipes") {
		std::vector<std::string> recipes = obj["content"]["enableCrafts"].get<std::vector<std::string>>();

		logs.push_back(ftxui::hflow({
			ftxui::text("Доступные крафты: ") | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		c = 1;

		for (auto it : recipes) {
			logs.push_back(ftxui::hflow({
				ftxui::text(std::to_string(c) + ". ") | ftxui::color(DECORATE_COLOR), ftxui::text(it) | ftxui::color(ITEM_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			++c;
		}
		return;
	}
	else if (obj["content"]["type"] == "Equip") {
		if (obj["content"]["canEquip"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вы не можете экипировать этот предмет") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["equiped"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Не найден предмет") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}

		logs.push_back(ftxui::hflow({
			ftxui::text("Вы экипировали ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(obj["content"]["wep"].get<std::string>()) | ftxui::color(ITEM_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		return;
	}
	else if (obj["content"]["type"] == "RepairExit") {
		if (obj["content"]["canFixThisRoom"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("В этой комнате нечего чинить") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["haveResources"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("У вас недостаточно ресурсов для починки") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вам необходимо: ") | ftxui::color(DECORATE_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

			nlohmann::json needrepair = obj["content"]["needRes"].get<nlohmann::json>();

			c = 1;

			for (auto it : needrepair) {
				logs.push_back(ftxui::hflow({
					ftxui::text(std::to_string(c) + ". ") | ftxui::color(DECORATE_COLOR), ftxui::text(it["name"].get<std::string>()) | ftxui::color(ITEM_COLOR),
					ftxui::text(" (") | ftxui::color(DECORATE_COLOR), ftxui::text(std::to_string(it["count"].get<int>())) | ftxui::color(DECORATE_COLOR), ftxui::text(" штуки)") | ftxui::color(DECORATE_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
				++c;
			}

			return;
		}
		if (obj["content"]["isExitBroken"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Выход не сломан") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}

		logs.push_back(ftxui::hflow({
				ftxui::paragraph("Вы починили выход!") | ftxui::color(ITEM_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		logs.push_back(ftxui::hflow({
			ftxui::paragraph("Вы потеряли: ") | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		std::vector<std::string> losteditems = obj["content"]["lostedItems"].get<std::vector<std::string>>();

		c = 1;

		for (auto it : losteditems) {
			logs.push_back(ftxui::hflow({
				ftxui::text(std::to_string(c) + ". ") | ftxui::color(DECORATE_COLOR), ftxui::text(it) | ftxui::color(ITEM_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			++c;
		}
	}
	else if (obj["content"]["type"] == "PlayersInRoom") {
		std::vector<std::string> plys = obj["content"]["players"].get<std::vector<std::string>>();

		logs.push_back(ftxui::hflow({
			ftxui::text("Игроки в этой комнате: ") | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		c = 1;

		for (auto it : plys) {
			logs.push_back(ftxui::hflow({
				ftxui::text(std::to_string(c) + ". ") | ftxui::color(DECORATE_COLOR), ftxui::text(it) | ftxui::color(ITEM_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			++c;
		}
		return;
	}
	else if (obj["content"]["type"] == "Battle") {
		if (obj["content"]["enemyInBattle"].get<bool>() == true) {
			logs.push_back(ftxui::hflow({
				ftxui::text(obj["content"]["enemyInBattlePlayerName"].get<std::string>()) | ftxui::color(ITEM_COLOR),
				ftxui::text(" уже дерётся с этим врагом!") | ftxui::color(DECORATE_COLOR),
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}
		if (obj["content"]["startedBattle"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Этого врага нет в комнате!") | ftxui::color(ENEMY_COLOR),
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			return;
		}

		logs.push_back(ftxui::hflow({
			ftxui::text("Вы вступили в битву с ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(obj["content"]["startedBattleWith"].get<std::string>()) | ftxui::color(ENEMY_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		//Check death

		nlohmann::json enemyStep = obj["content"]["enemyStep"].get<nlohmann::json>();

		if (!enemyStep.empty()) {
			addEnemyAttack(logs, enemyStep);
		}

		nlohmann::json playerDeath = obj["content"]["checkPlayerDeath"].get<nlohmann::json>();

		if (!playerDeath.empty()) {
			addPlayerDeath(logs, playerDeath);
		}

		return;
	}
	else if (obj["content"]["type"] == "Attack") {
		if (obj["content"]["inBattle"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::text("Вы не в битве!") | ftxui::color(ENEMY_COLOR),
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		if (obj["content"]["wepEquiped"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::text("У вас не экипировано оружие!") | ftxui::color(ENEMY_COLOR),
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
		else if (obj["content"]["inBattle"].get<bool>() == true) {
			logs.push_back(ftxui::hflow({
				ftxui::text("Вы нанесли врагу ") | ftxui::color(DECORATE_COLOR),
				ftxui::text(obj["content"]["enemyName"].get<std::string>()) | ftxui::color(ENEMY_COLOR),
				ftxui::text(" ") | ftxui::color(DECORATE_COLOR),
				ftxui::text(std::to_string(obj["content"]["makeDamage"].get<int>())) | ftxui::color(ITEM_COLOR),
				ftxui::text(" + ") | ftxui::color(DECORATE_COLOR),
				ftxui::text(std::to_string(obj["content"]["skillDamage"].get<int>())) | ftxui::color(ITEM_COLOR),
				ftxui::text(" урона (Осталось ") | ftxui::color(DECORATE_COLOR),
				ftxui::text(std::to_string(obj["content"]["enemyRemainHealth"].get<int>())) | ftxui::color(ENEMY_COLOR),
				ftxui::text(" здоровья)") | ftxui::color(DECORATE_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}

		if (obj["content"]["isEnemyDead"].get<bool>() == true) {
			logs.push_back(ftxui::hflow({
				ftxui::text("Вы победили и получили ") | ftxui::color(DECORATE_COLOR),
				ftxui::text(std::to_string(obj["content"]["winExp"].get<int>())) | ftxui::color(ITEM_COLOR),
				ftxui::text(" опыта") | ftxui::color(DECORATE_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

			if (obj["content"]["pickedUpWinItem"].get<bool>() == true) {
				logs.push_back(ftxui::hflow({
					ftxui::text("Вы получили с врага предмет ") | ftxui::color(DECORATE_COLOR),
					ftxui::text(obj["content"]["itemFromEnemy"].get<std::string>()) | ftxui::color(ITEM_COLOR),
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			}
			else {
				logs.push_back(ftxui::hflow({
					ftxui::paragraph("Вы не смогли получить с врага предмет, ваш инвентарь полон!") | ftxui::color(ENEMY_COLOR),
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			}
		}

		//Check death

		nlohmann::json enemyStep = obj["content"]["enemyStep"].get<nlohmann::json>();

		if (!enemyStep.empty()) {
			addEnemyAttack(logs, enemyStep);
		}

		nlohmann::json playerDeath = obj["content"]["checkPlayerDeath"].get<nlohmann::json>();

		if (!playerDeath.empty()) {
			addPlayerDeath(logs, playerDeath);
		}

		nlohmann::json lvUp = obj["content"]["levelUp"].get<nlohmann::json>();

		if (!lvUp.empty()) {
			addLevelUp(logs, lvUp);
		}
		return;
	}
	else if (obj["content"]["type"] == "Skip") {
		if (obj["content"]["inBattle"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::text("Вы не в битве") | ftxui::color(DECORATE_COLOR),
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
		else {
			nlohmann::json enemyStep = obj["content"]["enemyStep"].get<nlohmann::json>();

			if (!enemyStep.empty()) {
				addEnemyAttack(logs, enemyStep);
			}

			nlohmann::json playerDeath = obj["content"]["checkPlayerDeath"].get<nlohmann::json>();

			if (!playerDeath.empty()) {
				addPlayerDeath(logs, playerDeath);
			}
		}
		return;
	}
	else if (obj["content"]["type"] == "unknown") {
		logs.push_back(ftxui::hflow({
			ftxui::paragraph("Неизвестная команда!") | ftxui::color(ENEMY_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		return;
	}
	else if (obj["content"]["type"] == "inBattle") {
		logs.push_back(ftxui::hflow({
			ftxui::paragraph("Вы не можете использовать эту команду в бою!") | ftxui::color(ENEMY_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		return;
	}
	else if (obj["content"]["type"] == "badSyntax") {
		logs.push_back(ftxui::hflow({
			ftxui::paragraph("Неправильный синтаксис!") | ftxui::color(ENEMY_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		return;
	}
	else if (obj["content"]["type"] == "Help") {
		try {
			std::vector<nlohmann::json> helpList = obj["content"]["help"].get<std::vector<nlohmann::json>>();

			for (auto it : helpList) {
				logs.push_back(ftxui::hflow({
					ftxui::paragraph(it["name"].get<std::string>()) | ftxui::color(ITEM_COLOR),
					ftxui::text(" - ") | ftxui::color(DECORATE_COLOR),
					ftxui::paragraph(it["description"].get<std::string>()) | ftxui::color(DECORATE_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			}
		}
		catch (const std::exception& ec) {
			logs.push_back(ftxui::hflow({
				ftxui::text(ec.what()) | ftxui::color(ENEMY_COLOR),
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
	}
	else if (obj["content"]["type"] == "SessionError") {
		logs.push_back(ftxui::hflow({
			ftxui::paragraph(obj["content"]["text"]) | ftxui::color(ENEMY_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
	else if (obj["content"]["type"] == "SamosborDeath") {
		nlohmann::json playerDeath = obj["content"]["death"].get<nlohmann::json>();

		if (!playerDeath.empty()) {
			addPlayerDeath(logs, playerDeath);
		}
	}
	else if (obj["content"]["type"] == "Respawn") {
		logs.push_back(ftxui::hflow({
			ftxui::text("Вы возродились!") | ftxui::color(ITEM_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
	else if (obj["content"]["type"] == "youDead") {
		logs.push_back(ftxui::hflow({
			ftxui::text("Вы не можете использовать команды пока мертвы!") | ftxui::color(ENEMY_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
	else if (obj["content"]["type"] == "RecipesItem") {
		if (obj["content"]["foundItem"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Не найден предмет!") | ftxui::color(ENEMY_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
		else {
			std::vector<std::string> itemRecipeNames = obj["content"]["itemsToCraft"].get<std::vector<std::string>>();

			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Крафты с использованием этого предмета:") | ftxui::color(DECORATE_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

			c = 1;

			for (auto it : itemRecipeNames) {
				logs.push_back(ftxui::hflow({
					ftxui::paragraph(std::to_string(c) + ". " + it) | ftxui::color(ITEM_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
				++c;
			}
		}
	}
	else if (obj["content"]["type"] == "Recipe") {
		if (obj["content"]["foundItem"].get<bool>() == false) {
			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Не найден предмет!") | ftxui::color(ENEMY_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
		}
		else {
			if (obj["content"]["foundRecipe"].get<bool>() == false) {
				logs.push_back(ftxui::hflow({
					ftxui::paragraph("Этот предмет не крафтиться!") | ftxui::color(ENEMY_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
				return;
			}

			std::vector<std::string> RecipeNames = obj["content"]["recipe"].get<std::vector<std::string>>();

			logs.push_back(ftxui::hflow({
				ftxui::paragraph("Рецепт этого предмета:") | ftxui::color(DECORATE_COLOR)
			}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

			c = 1;

			for (auto it : RecipeNames) {
				logs.push_back(ftxui::hflow({
					ftxui::paragraph(std::to_string(c) + ". " + it) | ftxui::color(ITEM_COLOR)
				}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
				++c;
			}
		}
	}
	else if (obj["content"]["type"] == "Message") {
		logs.push_back(ftxui::hflow({
			ftxui::text(obj["content"]["from"].get<std::string>()) | ftxui::color(ITEM_COLOR),
			ftxui::text(" сказал: ") | ftxui::color(DECORATE_COLOR),
			ftxui::paragraph(obj["content"]["msg"].get<std::string>()) | ftxui::color(ADD_COLOR1)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
}

void addServerMsg(std::vector<ftxui::Element>& serverMessages, const nlohmann::json& obj) {
	//Clear log if big

	if (serverMessages.size() > MAX_SERVER_MSGS) {
		while (serverMessages.size() > MAX_SERVER_MSGS) {
			serverMessages.erase(serverMessages.begin());
		}
	}

	if (obj["content"]["type"] == "NewPlayer") {
		serverMessages.push_back(ftxui::hflow({
			ftxui::text("* Игрок ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(obj["content"]["name"].get<std::string>()) | ftxui::color(ITEM_COLOR),
			ftxui::text(" подключился! ") | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
	else if (obj["content"]["type"] == "PlayerDisconnect") {
		serverMessages.push_back(ftxui::hflow({
			ftxui::text("* Игрок ") | ftxui::color(DECORATE_COLOR),
			ftxui::text(obj["content"]["name"].get<std::string>()) | ftxui::color(ENEMY_COLOR),
			ftxui::text(" отключился! ") | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
	else if (obj["content"]["type"] == "SamosborWait") {
		serverMessages.push_back(ftxui::hflow({
			ftxui::paragraph("* Через") | ftxui::color(DECORATE_COLOR),
			ftxui::text(" ") | ftxui::color(DECORATE_COLOR),
			ftxui::paragraph(std::to_string(obj["content"]["time"].get<int>())) | ftxui::color(ENEMY_COLOR),
			ftxui::text(" ") | ftxui::color(DECORATE_COLOR),
			ftxui::paragraph("секунд начнётся самосбор! Спрячтесь в одном из укрытий чтобы не умереть: ") | ftxui::color(DECORATE_COLOR)
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

		std::vector<std::string> savePlaces = obj["content"]["savePlaces"].get<std::vector<std::string>>();
		std::string res = "";

		for (auto it : savePlaces) {
			res += it + " ";
		}

		serverMessages.push_back(ftxui::hflow({
			ftxui::paragraph(res) | ftxui::color(ITEM_COLOR),
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
	else if (obj["content"]["type"] == "SamosborStarted") {
		serverMessages.push_back(ftxui::hflow({
			ftxui::paragraph("* Начался самосбор! Переждите его в укрытие до его окончания!") | ftxui::color(ENEMY_COLOR),
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
	else if (obj["content"]["type"] == "SamosborEnded") {
		serverMessages.push_back(ftxui::hflow({
			ftxui::paragraph("* Самосбор завершился, вы можете выходить из укрытий!") | ftxui::color(ITEM_COLOR),
		}) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
	}
	else if (obj["content"]["type"] == "") {

	}
	else if (obj["content"]["type"] == "") {

	}
	else if (obj["content"]["type"] == "") {

	}
}