#include "Game.h"

namespace Gigahrush {
	std::string toLowerCase(std::string str) {
		std::string res = "";
		for (unsigned char c : str) {
			res += std::tolower(c);
		}
		return res;
	}

	void Configurator::LoadMapSize() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json SizeConfig = js.readFile("Config/MainConfig.json");
		config.mapSize.FloorCount = SizeConfig["FloorCount"];
		config.mapSize.X = SizeConfig["FloorSizeX"];
		config.mapSize.Y = SizeConfig["FloorSizeY"];
		config.maxInventorySize = SizeConfig["MaxInventory"];
		config.maxRoomItems = SizeConfig["MaxRoomItems"];

		std::cout << "Main config loaded" << std::endl;
	}

	void Configurator::LoadItems() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json ItemsConfig = js.readFile("Config/Items.json");

		for (int i = 0; i < ItemsConfig.size(); i++) {
			std::unique_ptr<Item> item;

			if (ItemsConfig[i]["type"] == 0) { //Component
				item = std::make_unique<Component>(
					ItemsConfig[i]["id"],
					ItemsConfig[i]["name"],
					ItemsConfig[i]["description"],
					ItemsConfig[i]["useDescription"],
					ItemsConfig[i]["canSpawn"]);
			}
			else if (ItemsConfig[i]["type"] == 1) {
				item = std::make_unique<Weapon>(
					ItemsConfig[i]["id"],
					ItemsConfig[i]["name"],
					ItemsConfig[i]["description"],
					ItemsConfig[i]["useDescription"],
					ItemsConfig[i]["canSpawn"],
					ItemsConfig[i]["damage"]);
			}
			else if (ItemsConfig[i]["type"] == 2) {
				item = std::make_unique<Armor>(
					ItemsConfig[i]["id"],
					ItemsConfig[i]["name"],
					ItemsConfig[i]["description"],
					ItemsConfig[i]["useDescription"],
					ItemsConfig[i]["canSpawn"],
					ItemsConfig[i]["armor"]);
			}
			else if (ItemsConfig[i]["type"] == 3) {
				item = std::make_unique<HealingItem>(
					ItemsConfig[i]["id"],
					ItemsConfig[i]["name"],
					ItemsConfig[i]["description"],
					ItemsConfig[i]["useDescription"],
					ItemsConfig[i]["canSpawn"],
					ItemsConfig[i]["heal"]);
			}

			config.items.push_back(std::move(item));
			if (ItemsConfig[i]["canSpawn"] == true) {
				config.itemSpawnChances.push_back(SpawnChance(ItemsConfig[i]["id"], ItemsConfig[i]["spawnChance"]));
			}
		}

		std::cout << config.items.size() << " items loaded" << std::endl;
	}

	void Configurator::LoadEnemies() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json EnemiesConfig = js.readFile("Config/Enemies.json");

		for (int i = 0; i < EnemiesConfig.size(); i++) {
			std::vector<std::unique_ptr<Item>> loot;

			for (int j = 0; j < EnemiesConfig[i]["loot"].size(); j++) {
				for (int v = 0; v < config.items.size(); v++) {
					if (config.items[v]->ID == EnemiesConfig[i]["loot"][j]) {
						loot.push_back(config.items[v]->clone());
					}
				}
			}

			config.enemies.push_back(std::make_shared<Enemy>(
				EnemiesConfig[i]["id"],
				EnemiesConfig[i]["name"],
				EnemiesConfig[i]["description"],
				EnemiesConfig[i]["replics"],
				EnemiesConfig[i]["health"],
				EnemiesConfig[i]["attack"], 
				std::move(loot),
				EnemiesConfig[i]["exp"]
			));
			config.enemySpawnChances.push_back(SpawnChance(EnemiesConfig[i]["id"], EnemiesConfig[i]["spawnChance"]));
		}

		std::cout << config.enemies.size() <<  " enemies loaded" << std::endl;
	}

	void Configurator::LoadRooms() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json RoomsConfig = js.readFile("Config/Rooms.json");

		for (int i = 0; i < RoomsConfig.size(); i++) {
			config.rooms.push_back(std::make_unique<Room>(
				RoomsConfig[i]["id"],
				RoomsConfig[i]["name"],
				std::string{RoomsConfig[i]["description"]},
				std::vector<RoomDescElement>{},
				std::vector<RoomDescElement>{},
				std::vector<std::unique_ptr<Item>>{},
				std::vector<std::shared_ptr<Enemy>>{},
				RoomsConfig[i]["isExit"],
				Location()
			));

			config.roomDescs.push_back(RoomDesc(
				RoomsConfig[i]["id"],
				RoomsConfig[i]["itemDescs"],
				RoomsConfig[i]["enemiesDescs"]
			));
			config.roomSpawnChances.push_back(SpawnChance(RoomsConfig[i]["id"], RoomsConfig[i]["spawnChance"]));
		}
		std::cout << config.rooms.size() <<  " rooms loaded" << std::endl;
	}

	void Configurator::LoadCrafts() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json CraftsConfig = js.readFile("Config/Crafts.json");

		for (int i = 0; i < CraftsConfig.size(); i++) {
			config.crafts.push_back(Craft(
				CraftsConfig[i]["id"],
				CraftsConfig[i]["craft"]
			));
		}

		std::cout << config.crafts.size() <<  " crafts loaded" << std::endl;
	}

	void Configurator::ShowAllConfig() {
		std::cout << "=== All game config: ===\n\n";
		//MapSize

		std::cout << "=== Base settings: ===\n\n";

		std::cout << "Map Size: \n";
		std::cout << "FloorCount: " << config.mapSize.FloorCount << std::endl;
		std::cout << "FloorSizeX: " << config.mapSize.X << std::endl;
		std::cout << "FloorSizeY: " << config.mapSize.Y << std::endl << std::endl;
		std::cout << "Max inventory size: " << config.maxInventorySize << std::endl;
		std::cout << "Max room items: " << config.maxRoomItems << std::endl << std::endl;

		//Items
		
		std::cout << "=== All items: === \n\n";
		
		std:size_t count = 1;

		for (auto& it : config.items) {

			float spc = 0.0f;
			for (auto spawnchance : config.itemSpawnChances) {
				if (spawnchance.ID == it->ID) {
					spc = spawnchance.chance;
					break;
				}
			}

			std::cout << "# Item " << count << "\n\n";

			std::cout << "ID: " << it->ID << "\nName: " << it->name << "\nDescription: " 
				<< it->description  << "\nSpawn chance: " << spc << "\nCan spawn: " << it->canSpawn << "\n\n";
			++count;
		}

		//Enemies
		
		std::cout << "=== All enemies: ===\n\n";

		count = 1;

		for (auto& it : config.enemies) {

			float spc = 0.0f;
			for (auto spawnchance : config.enemySpawnChances) {
				if (spawnchance.ID == it->ID) {
					spc = spawnchance.chance;
					break;
				}
			}

			std::cout << "# Enemy " << count << "\n\n";

			std::cout << "ID: " << it->ID << "\nName: " << it->name << "\nDescription: "
				<< it->description << "\nHealth: "
				<< it->health << "\nAttack: "
				<< it->attack << "\nSpawn chance: " << spc << "\n\n";

			std::cout << "--- Replics: ---\n\n";

			std::size_t count2 = 1;
			for (auto r : it->replics) {
				std::cout << count2 << r << "\n";
				++count2;
			}

			std::cout << "\n--- Loot: ---\n\n";
			for (auto &r : it->loot) {
				std::cout << r->name << "\n";
			}
			std::cout << "\n";
			++count;
		}

		//Rooms
		
		std::cout << "=== All rooms: ===\n\n";

		count = 1;

		for (auto& it : config.rooms) {

			float spc = 0.0f;
			for (auto spawnchance : config.roomSpawnChances) {
				if (spawnchance.ID == it->ID) {
					spc = spawnchance.chance;
					break;
				}
			}

			std::cout << "# Room " << count << "\n\n";

			std::cout << "ID: " << it->ID << "\nName: " << it->name << "\nDescription: " << it->description << "\nSpawn chance: " << spc << "\n";
			for (auto ds : config.roomDescs) {
				if (ds.ID == it->ID) {
					std::cout << "\n--- Item description: ---\n\n";
					for (auto ids : ds.itemDescs) {
						std::cout << ids << "\n";
					}
					std::cout << "\n--- Enemy description: ---\n\n";
					for (auto eds : ds.enemiesDescs) {
						std::cout << eds << "\n";
					}
				}
			}
			std::cout << "\n";
			++count;
		}

		//Crafts

		std::cout << "All crafts: \n\n";

		count = 1;

		for (auto it : config.crafts) {
			std::cout << "# Craft " << count << "\n\n";

			std::string craftName = "Unknown item";
			for (auto& tt : config.items) {
				if (tt->ID == it.ID) {
					craftName = tt->name;
				}
			}
			std::cout << "Item name: " << craftName << "\n";
			std::cout << "ID: " << it.ID << "\nCraft: ";
			for (int i = 0; i < it.craft.size(); i++) {
				std::string name = "Unknown ID";
				for (auto& tt : config.items) {
					if (tt->ID == it.craft[i]) {
						name = tt->name;
					}
				}
				std::cout << name << ", ";
			}
			++count;
		}
		std::cout << "\n";
	}

	bool Configurator::LoadConfig() {
		config = Config();
		std::cout << "Start loading config" << std::endl;

		try {
			LoadMapSize();
			LoadItems();
			LoadEnemies();
			LoadRooms();
			LoadCrafts();
			config.configLoaded = true;
			std::cout << "Config loaded!\n";

			return true;
		}
		catch (std::exception& e) {
			std::cout << e.what() << "\n";
		}
		//ShowAllConfig();
	}

	Game& Game::Instance() {
		static Game g;
		return g;
	}

	Game::Game(): isGenerated(false) {}
	Game::~Game() = default;

	bool Game::changeDir(std::vector<std::vector<int>>& mask, int& X, int& Y, int& randDir) {
		randDir = (rand() % 4) + 1;

		if (X >= 0 && X < mask.size() && Y >= 0 && Y < mask[0].size()) {
			if (mask[Y][X] == 0) { return true; }
		}

		int tempX = X;
		int tempY = Y;

		switch (randDir) {
			case 1:
				tempY -= 1;
				break;
			case 2:
				tempY += 1;
				break;
			case 3:
				tempX -= 1;
				break;
			case 4:
				tempX += 1;
				break;
		}

		if (tempX >= 0 && tempX < mask.size() && tempY >= 0 && tempY < mask[0].size()) {
			X = tempX;
			Y = tempY;

			return true;
		}
		else {
			return false;
		}

	}

	void Game::GenerateBranchMask(std::vector<std::vector<int>>& mask, int currentX, int currentY, int remainingSteps, int currentDir) {
		if (remainingSteps <= 0 || currentX < 0 || currentX >= mask[0].size() || currentY < 0 || currentY >= mask.size()) {
			return;
		}

		if (mask[currentY][currentX] == 1) { return; }
		mask[currentY][currentX] = 1;

		if (rand() % 100 < 50) {
			GenerateBranchMask(mask, currentX, currentY, remainingSteps, (rand() % 4) + 1);
		}

		if (rand() % 100 < 65) {
			int newX = currentX;
			int newY = currentY;

			switch (currentDir) {
				case 1:
					newY += 1;
					break;
				case 2:
					newY -= 1;
					break;
				case 3:
					newX -= 1;
					break;
				case 4:
					newX += 1;
					break;
			}

			if (newX >= 0 && newX < mask[0].size() && newY >= 0 && newY < mask.size()) {
				GenerateFloorMask(mask, newX, newY, remainingSteps - 1, currentDir);
			}
		}
		else {
			for (int i = 1; i <= 4; i++) {
				if (remainingSteps <= 0) { break; }

				int randDir = (rand() % 4) + 1;

				if (rand() % 100 < 55) {
					int newX = currentX;
					int newY = currentY;

					switch (randDir) {
						case 1:
							newY += 1;
							break;
						case 2:
							newY -= 1;
							break;
						case 3:
							newX -= 1;
							break;
						case 4:
							newX += 1;
							break;
					}

					for (int z = 0; z < 5; z++) {
						changeDir(mask, newX, newY, randDir);
					}

					if (newX >= 0 && newX < mask[0].size() && newY >= 0 && newY < mask.size()) {
						GenerateFloorMask(mask, newX, newY, remainingSteps - 1, randDir);
					}
				}
			}
		}
	}

	void Game::GenerateFloorMask(std::vector<std::vector<int>>& mask, int currentX, int currentY, int remainingSteps, int currentDir) {
		if (remainingSteps <= 0 || currentX < 0 || currentX >= mask[0].size() || currentY < 0 || currentY >= mask.size()) {
			return;
		}

		if (mask[currentY][currentX] == 1) { return; }
		mask[currentY][currentX] = 1;

		if (rand() % 100 < 50) {
			GenerateBranchMask(mask, currentX, currentY, remainingSteps, (rand() % 4) + 1);
		}

		if (rand() % 100 < 75) {
			int newX = currentX;
			int newY = currentY;

			switch (currentDir) {
				case 1:
					newY += 1;
					break;
				case 2:
					newY -= 1;
					break;
				case 3:
					newX -= 1;
					break;
				case 4:
					newX += 1;
					break;
			}

			if (newX >= 0 && newX <= mask[0].size() && newY >= 0 && newY <= mask.size()) {
				GenerateFloorMask(mask, newX, newY, remainingSteps - 1, currentDir);
			}
		}
		else {
			for (int i = 1; i <= 4; i++) {
				if (remainingSteps <= 0) { break; }

				int randDir = (rand() % 4) + 1;

				if (rand() % 100 < 45) {
					int newX = currentX;
					int newY = currentY;

					switch (randDir) {
						case 1:
							newY += 1;
							break;
						case 2:
							newY -= 1;
							break;
						case 3:
							newX -= 1;
							break;
						case 4:
							newX += 1;
							break;
					}

					for (int z = 0; z < 5; z++) {
						changeDir(mask, newX, newY, randDir);
					}

					if (newX >= 0 && newX <= mask[0].size() && newY >= 0 && newY <= mask.size()) {
						GenerateFloorMask(mask, newX, newY, remainingSteps - 1, randDir);
					}
				}
			}
		}
	}

	void Game::PrintRoomInfo(std::shared_ptr<Room>& rm) {
		std::cout << "\n# Room " << rm->ID << "\n\n";
		std::cout << "ID: " << rm->ID << "\nName: " << rm->name
			<< "\nDescription: " << rm->description << "\n"
			<< "Is exit: " << rm->isExit
			<< "\nLocation: " << rm->location.X << " " << rm->location.Y << " " << rm->location.F << "\n";
		std::cout << "\n";

		std::cout << "Items:\n\n";

		for (auto& it : rm->items) {
			std::cout << "Name: " << it->name << "\n";
		}

		std::cout << "--- Items room descs ---\n\n";

		for (auto& t : rm->itemDescription) {
			std::cout << "Room description: " << t.desc << "\n";
		}

		std::cout << "Enemies:\n\n";

		for (auto& it : rm->enemies) {
			std::cout << "Name: " << it->name << "\n";
		}

		std::cout << "--- Enemy room descs ---\n\n";

		for (auto& t : rm->enemyDescription) {
			std::cout << "Room description: " << t.desc << "\n";
		}
	}

	void Game::GenerateItemsAndEnemies(std::shared_ptr<Room>& rm) {
		//Gen items
		std::vector<RoomDescElement> itemDescription;
		std::vector<std::unique_ptr<Item>> items;

		int itemCount = (rand() % configurator.config.maxRoomItems) + 1;

		for (int i = 0; i < itemCount; i++) {
			int itemId = 0;

			//Random algoritm
			int Weight = 0;

			for (auto &ch : configurator.config.itemSpawnChances) {
				Weight += (ch.chance * 100);
			}

			int choose = rand() % Weight;

			Weight = 0;

			for (auto &ch : configurator.config.itemSpawnChances) {
				Weight += (ch.chance * 100);
				if (Weight >= choose) {
					itemId = ch.ID;
					break;
				}
			}

			std::string itName = "";
			//End random
			for (auto &it : configurator.config.items) {
				if (it->ID == itemId) {
					itName = it->name;
					items.push_back(it->clone());
					break;
				}
			}

			std::string phrase = "";

			for (auto &it : configurator.config.roomDescs) {
				if (rm->ID == it.ID) {
					if (it.itemDescs.size() == 1) {
						phrase = std::vformat(std::string_view(it.itemDescs[0]), std::make_format_args(itName));
					}
					else {
						phrase = std::vformat(std::string_view(it.itemDescs[rand() % it.itemDescs.size()]), std::make_format_args(itName));
					}

					//phrase = it.itemDescs[rand() % it.itemDescs.size()];
					//sprintf(&phrase[0], itName.c_str());
				}
			}

			itemDescription.push_back(RoomDescElement(itemId, phrase));
		}

		//Gen enemy
		std::vector<RoomDescElement> enemyDescription;
		std::vector<std::shared_ptr<Enemy>> enemies;

		if (rand() % 100 <= 25) {
			int enemyCount = (rand() % (configurator.config.maxRoomItems / 2)) + 1;
			for (int i = 0; i < enemyCount; i++) {
				int enemyID = 0;

				//Random algoritm
				int Weight = 0;

				for (auto &ch : configurator.config.enemySpawnChances) {
					Weight += (ch.chance * 100);
				}

				int choose = rand() % Weight;

				Weight = 0;

				for (auto &ch : configurator.config.enemySpawnChances) {
					Weight += (ch.chance * 100);
					if (Weight >= choose) {
						enemyID = ch.ID;
						break;
					}
				}
				//End random

				std::string enName = "";

				for (auto& it : configurator.config.enemies) {
					if (it->ID == enemyID) {
						enName = it->name;
						enemies.push_back(it->clone());
						break;
					}
				}
				std::string phrase = "";

				for (auto& it : configurator.config.roomDescs) {
					if (rm->ID == it.ID) {
						if (it.enemiesDescs.size() == 1) {
							phrase = std::vformat(std::string_view(it.enemiesDescs[0]), std::make_format_args(enName));
						}
						else {
							phrase = std::vformat(std::string_view(it.enemiesDescs[rand() % it.enemiesDescs.size()]), std::make_format_args(enName));
						}
						//phrase = it.enemiesDescs[rand() % it.enemiesDescs.size()];
						//sprintf(&phrase[0], enName.c_str());
						//phrase = std::format(it.enemiesDescs[rand() % it.enemiesDescs.size()], enName);
					}
				}

				enemyDescription.push_back(RoomDescElement(enemyID, phrase));
			}
		}

		rm->itemDescription = itemDescription;
		rm->enemyDescription = enemyDescription;

		rm->items = std::move(items);
		rm->enemies = enemies;
	}

	std::shared_ptr<Room> Game::GenerateRoom(Location loc, bool isExit) {
		int RoomID = 0;

		//Random algoritm
		int Weight = 0;

		for (auto ch : configurator.config.roomSpawnChances) {
			Weight += (ch.chance * 100);
		}

		int choose = rand() % Weight;

		Weight = 0;

		for (auto ch : configurator.config.roomSpawnChances) {
			Weight += (ch.chance * 100);
			if (Weight >= choose) {
				RoomID = ch.ID;
				break;
			}
		}
		//End random
		std::shared_ptr<Room> room;

		for (auto& rm : configurator.config.rooms) {
			if (rm->ID == RoomID) {
				room = rm->clone();
			}
		}

		room->isExit = isExit;
		room->location = loc;

		GenerateItemsAndEnemies(room);
		//PrintRoomInfo(room);
		return room;
	}

	void Game::GenerateFloors() {
		//std::cout << "\nStarted generate floors.\n";
		for (int i = 1; i <= configurator.config.mapSize.FloorCount; i++) {
			//std::cout << "\nFloor - " << i << "\n\n";

			std::shared_ptr<Floor> flr = std::make_shared<Floor>(
				0,
				std::vector<std::shared_ptr<Room>>{},
				std::vector<std::vector<int>>{},
				Location{ 0, 0, 0 },
				true,
				true
			);

			if (i == 1) {
				flr->canGoDown = false;
			}

			if (i == configurator.config.mapSize.FloorCount) {
				flr->canGoUp = false;
			}

			flr->level = i;
			//std::vector<std::vector<int>> mask(configurator.config.mapSize.X, std::vector<int>(configurator.config.mapSize.Y,0));

			flr->floorMask.resize(configurator.config.mapSize.Y, std::vector<int>(configurator.config.mapSize.X, 0));

			int randStartX = configurator.config.mapSize.X/2;
			int randStartY = configurator.config.mapSize.Y/2;

			flr->exitCoordinates = Location(randStartX, randStartY, i);

			int remainingSteps = configurator.config.mapSize.X;

			GenerateFloorMask(flr->floorMask, randStartX, randStartY, remainingSteps, (rand()%4)+1);

			for (int y = 0; y < flr->floorMask.size(); y++) {
				for (int x = 0; x < flr->floorMask[0].size(); x++) {
					if (flr->floorMask[y][x] == 1) {
						bool isex;
						Location loc(x, y, i);

						if (loc == flr->exitCoordinates) { isex = true; }
						else { isex = false; }

						flr->rooms.push_back(GenerateRoom(loc,isex));
					}
					else {
					}
				}
			}
			gamedata.floors.push_back(flr);
		}
	}

	bool Game::GenerateGame() {
		gamedata = GameData();
		std::cout << "Generating game...\n";
		if (configurator.config.configLoaded == true && isGenerated == false) {
			auto start = std::chrono::high_resolution_clock::now();
			GenerateFloors();
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			std::cout << "Game generated in " << duration << ", server now can be started!\n";
			isGenerated = true;
			return true;
		}
		else {
			if (isGenerated) {
				std::cout << "Can't generate game, already generated. Please type 'reset' for reset game.\n";
				return false;
			}

			std::cout << "Can't generate game, config is not loaded\n";
			return false;
		}
	}

	void Game::ResetGame() {
		gamedata = GameData();
		isGenerated = false;
	}

	void Game::Info() {
		if (isGenerated) {
			size_t rooms = 0;
			size_t items = 0;
			size_t enemies = 0;

			for (auto& it : gamedata.floors) {
				rooms += it->rooms.size();
				for (auto& lol : it->rooms) {
					items += lol->items.size();
				}
				for (auto& lol : it->rooms) {
					enemies += lol->enemies.size();
				}
			}

			std::cout << "=== GAME INFO ===\n";
			std::cout << "Floors count: " << gamedata.floors.size() << "\n";
			std::cout << "Rooms count: " << rooms << "\n";
			std::cout << "Items count: " << items << "\n";
			std::cout << "Enemies count: " << enemies << "\n";
			std::cout << "Players on server: " << gamedata.players.size() << "\n";
		}
		else {
			std::cout << "No info. Game is not generated or loaded\n";
		}
	}

	std::shared_ptr<Player> Game::SpawnPlayer(std::string& name) {
		std::shared_ptr<Player> ply = std::make_shared<Player>();
		ply->username = name;
		//Random spawning

		ply->battleStatus.status = NotInBattle;
		ply->stats = PlayerStats(100, 0, 1, configurator.config.maxInventorySize, false, 0, 0); //health,armor,level, max inv size, is weapon eq, weapon id, weaponskills

		for (auto& it : gamedata.floors) {
			if (it->level == 1) {
				ply->floor = it;
				ply->location = it->rooms[rand() % it->rooms.size()];
			}
		}

		gamedata.players.push_back(ply);
		return ply;
	}

	std::string Game::Me(std::shared_ptr<Gigahrush::Player> ply) {
		std::string res = "Имя: " + ply->username + "\n" +
			"Уровень: " + std::to_string(ply->stats.level) + "\n" +
			"Вместимость инвентаря: " + std::to_string(ply->stats.inventoryMaxSize) + "\n" +
			"Здоровье: " + std::to_string(ply->stats.health) + "\n" +
			"Броня: " + std::to_string(ply->stats.armor) + "\n" +
			"Навык владения оружием: " + std::to_string(ply->stats.weaponSkill);

		if (ply->stats.wepEq) {
			for (auto& it : ply->inventory) {
				if (it->ID == ply->stats.weaponEqID) {
					res += "\nТекущее оружие: " + it->name;
				}
			}
		}

		return res;
	}

	std::string Game::Map(std::shared_ptr<Player> ply) {
		std::string res = "@ - Вы\n0 - Выход на этажи\n\n"; //"Карта:\n";

		for (int y = 0; y < ply->floor->floorMask.size(); y++) {
			for (int x = 0; x < ply->floor->floorMask[0].size(); x++) {
				if (ply->floor->floorMask[y][x] == 0) {res += " ";}
				else {
					if (x == ply->location->location.X && y == ply->location->location.Y) { res += "@"; }
					else {
						if (x == ply->floor->exitCoordinates.X && y == ply->floor->exitCoordinates.Y){res += "0";}
						else {res += "#";}
					}
				}
			}
			res += "\n";
		}

		return res;
	}

	std::string Game::Look(std::shared_ptr<Gigahrush::Player> ply) {
		std::string res = ply->location->name + ": Этаж " + std::to_string(ply->floor->level) + "\n" +
			"Координаты: [" + std::to_string(ply->location->location.X) + ", " + std::to_string(ply->location->location.Y) + "]\n"+
			ply->location->description + "\n";
		for (auto& it : ply->location->itemDescription) {
			res += it.desc + "\n";
		}
		for (auto& it : ply->location->enemyDescription) {
			res += it.desc + "\n";
		}
		res += "Вы можете пойти на ";

		std::vector<std::vector<int>>& mask = ply->floor->floorMask;
		int plyX = ply->location->location.X;
		int plyY = ply->location->location.Y;

		if ((plyY - 1 >= 0 && plyY - 1 < mask.size() && plyX >= 0 && plyX < mask[0].size()) && mask[plyY-1][plyX] == 1) { res += "север "; }
		if ((plyY + 1 >= 0 && plyY + 1 < mask.size() && plyX >= 0 && plyX < mask[0].size()) && mask[plyY+1][plyX] == 1) { res += "юг "; }
		if ((plyY >= 0 && plyY < mask.size() && plyX - 1 >= 0 && plyX - 1 < mask[0].size()) && mask[plyY][plyX-1] == 1) { res += "запад "; }
		if ((plyY >= 0 && plyY < mask.size() && plyX + 1 >= 0 && plyX + 1 < mask[0].size()) && mask[plyY][plyX+1] == 1) { res += "восток "; }

		if (ply->location->isExit == true) {
			if (ply->floor->canGoUp == true) {
				res += "\nИз этой локации вы можете подняться на этаж выше";
			}
			if (ply->floor->canGoDown == true) {
				res += "\nИз этой локации вы можете опуститься на этаж ниже";
			}
		}

		return res;
	}

	std::string Game::Move(std::shared_ptr<Gigahrush::Player> ply, std::string side) {
		std::string res = "";
		std::vector<std::vector<int>> mask = ply->floor->floorMask;

		int posX = ply->location->location.X;
		int posY = ply->location->location.Y;

		if (side == "север") {posY -= 1;}
		else if (side == "юг") {posY += 1;}
		else if (side == "запад") {posX -= 1;}
		else if (side == "восток") {posX += 1;}
		else { return "Неизвестная сторона"; }

		if (posY < 0 || posY >= mask.size() || posX < 0 || posX >= mask[0].size()) {return "Вы не можете пойти в эту сторону";}
		if (mask[posY][posX] != 1) { return "Вы не можете пойти в эту сторону"; }

		for (auto& it : ply->floor->rooms) {
			if (it->location.X == posX && it->location.Y == posY) {
				ply->location = it;
				res = "Вы переместились\n";
				res += Look(ply);
				return res;
			}
		}

		return "Вы не можете пойти в эту сторону";
	}

	std::string Game::Craft(std::shared_ptr<Gigahrush::Player> ply, std::string item) {
		std::string res = "";
		std::unordered_map<int, int> craft; //id - count;
		std::unordered_map<int, int> enable; //id - count;
		std::unique_ptr<Item> itemToCraft = nullptr;
		bool itemFound = false;
		bool hasCraft = false;
		for (auto& it : configurator.config.items) {
			if (it->name == item) {
				itemToCraft = it->clone();
				itemFound = true;
				break;
			}
		}

		if (itemFound) {
			for (auto it : configurator.config.crafts) {
				if (it.ID == itemToCraft->ID) {
					for (auto xd : it.craft) {
						++craft[xd];
						hasCraft = true;
					}
					break;
				}
				else {
					hasCraft = false;
				}
			}
			if (hasCraft) {
				//Ищем предметы в инвентаре
				for (auto& it : ply->inventory) {
					++enable[it->ID];
				}
				//Сравниваем с крафтом
				bool canCraft = true;

				for (auto [id, count] : craft) {
					if (enable[id] < count) {
						canCraft = false;
						break;
					}
				}

				if (canCraft) {
					for (auto [id, count] : craft) {
						int delRemain = count;

						for (int i = ply->inventory.size() - 1; i >= 0 && delRemain > 0; i--) {
							if (ply->inventory[i]->ID == id) {
								res = "\nВы потеряли " + ply->inventory[i]->name;
								ply->inventory.erase(ply->inventory.begin() + i);
								--delRemain;
							}
						}
					}
					res = "Вы успешно скрафтили предмет - " + itemToCraft->name;
					ply->inventory.push_back(std::move(itemToCraft));
				}
				else {
					res = "Недостаточно ресурсов для крафта";
				}
			}
			else {
				res = "У этого предмета нет крафта";
			}
		}
		else {
			res = "Предмет не найден";
		}

		return res;
	}

	std::string Game::DropItem(std::shared_ptr<Gigahrush::Player> ply, std::string item) {
		std::string res = "У вас нет этого предмета в инвентаре";
		for (int i = 0; i < ply->inventory.size(); i++) {
			if (ply->inventory[i]->name == item) {
				if (ply->inventory[i]->ID == ply->stats.weaponEqID) {
					ply->stats.weaponEqID = 0;
					ply->stats.wepEq = false;
				}

				ply->location->items.push_back(ply->inventory[i]->clone());
				res = "Вы выбросили " + ply->inventory[i]->name;
				std::string phrase = "";
				for (auto& it : configurator.config.roomDescs) {
					if (ply->location->ID == it.ID) {
						if (it.itemDescs.size() == 1) {
							phrase = std::vformat(std::string_view(it.itemDescs[0]), std::make_format_args(item));
						}
						else {
							phrase = std::vformat(std::string_view(it.itemDescs[rand() % it.itemDescs.size()]), std::make_format_args(item));
						}
					}
				}
				ply->location->itemDescription.push_back(RoomDescElement(ply->inventory[i]->ID, phrase));
				ply->inventory.erase(ply->inventory.begin() + i);
				break;
			}
		}
		return res;
	}

	std::string Game::PickupItem(std::shared_ptr<Gigahrush::Player> ply, std::string item) {
		std::string res = "Этого предмета нет в комнате";

		if (ply->location->enemies.size() != 0) {
			return "Вы не можете подбирать предметы пока в комнате есть враги!";
		}

		bool isFound = false;
		for (int i = 0; i < ply->location->items.size(); i++) {
			if (ply->location->items[i]->name == item) {
				for (int j = 0; j < ply->location->itemDescription.size(); j++) {
					if (ply->location->itemDescription[j].ID == ply->location->items[i]->ID) {
						if (ply->inventory.size() < ply->stats.inventoryMaxSize) {
							ply->inventory.push_back(ply->location->items[i]->clone());
							res = "Вы подняли " + ply->location->items[i]->name + "";
							ply->location->items.erase(ply->location->items.begin() + i);
							ply->location->itemDescription.erase(ply->location->itemDescription.begin() + j);
							isFound = true;
							break;
						}
						else {
							res = "Ваш инвентарь полон";
							break;
						}
					}
				}
			}
			if (isFound) {
				break;
			}
		}

		return res;
	}

	std::string Game::Inventory(std::shared_ptr<Player> ply) {
		std::string res = "Ваш инвентарь:";
		size_t i = 1;

		for (auto& it : ply->inventory) {
			res += "\n" + std::to_string(i) + ". " + it->name;
			++i;
		}

		return res;
	}

	std::string Game::ChangeFloor(std::shared_ptr<Player> ply, int dir) { 
		//1 - up 0 - down
		std::string res = "";
		std::shared_ptr<Floor> flrToChange = nullptr;
		std::shared_ptr<Room> roomToChange = nullptr;

		bool canChange = false;

		if (ply->location->isExit == true) {
			if (dir == 0)
				if (ply->floor->canGoDown == true) {
					for (auto& it : gamedata.floors) {
						if (it->level == ply->floor->level-1) {
							flrToChange = it;
							for (auto& at : it->rooms) {
								if (at->location == it->exitCoordinates) {
									roomToChange = at;
									canChange = true;
									res = "Вы опустились на " + std::to_string(it->level)+ " этаж";
								}
							}
						}
					}
				}
				else {
					res = "Вы не можете спуститься ниже";
				}
			else if (dir == 1) {
				if (ply->floor->canGoUp == true) {
					for (auto& it : gamedata.floors) {
						if (it->level == ply->floor->level + 1) {
							flrToChange = it;
							for (auto& at : it->rooms) {
								if (at->location == it->exitCoordinates) {
									roomToChange = at;
									canChange = true;
									res = "Вы поднялись на " + std::to_string(it->level) + " этаж";
								}
							}
						}
					}
				}
				else {
					res = "Вы не можете подняться ниже";
				}
			}
		}
		else {
			res = "Вы не можете поменять этаж в этой комнате";
		}

		if (canChange) {
			ply->location = roomToChange;
			ply->floor = flrToChange;
		}

		return res;
	}

	std::string Game::LookItem(std::shared_ptr<Player> ply, std::string item) {
		std::string res = "У вас не такого предмета";

		for (auto& it : ply->inventory) {
			if (it->name == item) {
				res = it->getDescription();
				break;
			}
		}

		return res;
	}

	std::string Game::EnableCrafts(std::shared_ptr<Player> ply) {
		std::string res = "Доступные крафты: ";

		std::unordered_map<int, int> plyInv;
		std::vector<int> recipesCanBeCrafted;

		for (auto& it : ply->inventory) {
			++plyInv[it->ID];
		}

		for (auto it : configurator.config.crafts) {
			bool canCraft = true;
			std::unordered_map<int, int> recipe;
			for (auto& cr : it.craft) {
				++recipe[cr];
			}

			for (auto [id, count] : recipe) {
				if (plyInv[id] < count) { canCraft = false; break; };
			}

			if (canCraft) {
				recipesCanBeCrafted.push_back(it.ID);
			}
		}
		size_t counter = 1;
		for (auto x : recipesCanBeCrafted) {
			for (auto& it : configurator.config.items) {
				if (x == it->ID) {
					res += "\n" + std::to_string(counter) + ". " + it->name;
					break;
				}
			}
		}

		return res;
	}

	std::string Game::UseItem(std::shared_ptr<Player> ply, std::string item) {
		std::string res = "У вас нет этого предмета";
		for (int i = 0; i < ply->inventory.size(); i++) {
			if (ply->inventory[i]->name == item) {
				std::pair<std::string,bool> useRes = ply->inventory[i]->use(ply);
				res = useRes.first;
				if (useRes.second == true) {
					ply->inventory.erase(ply->inventory.begin() + i);
				}
				break;
			}
		}

		if (ply->battleStatus.status == InBattle) {
			res += ply->battleStatus.enemy->Attack(ply);
			res += CheckPlayerDeath(ply);
		}

		return res;
	}

	std::string Game::Attack(std::shared_ptr<Player> ply) {
		if (ply->battleStatus.status != InBattle) { return "Вы не в битве"; }

		std::string res = "У вас не экипировано оружие";
		bool isEnemyDead = false;

		if (ply->stats.wepEq == false) {
			if (isEnemyDead == false) {
				res += ply->battleStatus.enemy->Attack(ply);
				res += CheckPlayerDeath(ply);
			}

			return res;
		}

		for (auto& it : ply->inventory) {
			if (it->ID == ply->stats.weaponEqID) {
				Weapon* wep = dynamic_cast<Weapon*>(it.get());
				if (wep != nullptr) {
					ply->battleStatus.enemy->health = std::clamp(ply->battleStatus.enemy->health - (wep->damage + ply->stats.weaponSkill), 0, 1000);
					res = "Вы нанесли врагу " + ply->battleStatus.enemy->name + " " + std::to_string(wep->damage) + " + " + std::to_string(ply->stats.weaponSkill) + " урона(Осталось " + std::to_string(ply->battleStatus.enemy->health) + " здоровья).";
					if (ply->battleStatus.enemy->health <= 0) {
						ply->stats.level += ply->battleStatus.enemy->exp;
						ply->stats.weaponSkill += 1;
						ply->stats.inventoryMaxSize += 1;
						isEnemyDead = true;
						res += "\nВы победили " + ply->battleStatus.enemy->name + " и получили " + std::to_string(ply->battleStatus.enemy->exp) + " опыта.";
						int randItemFromEnemyID = 0;

						if (ply->battleStatus.enemy->loot.size() != 1) {
							randItemFromEnemyID = ply->battleStatus.enemy->loot[rand() % (ply->battleStatus.enemy->loot.size() - 1)]->ID;
						}
						else {
							randItemFromEnemyID = ply->battleStatus.enemy->loot[0]->ID;
						}

						for (auto& it : configurator.config.items) {
							if (it->ID == randItemFromEnemyID) {
								ply->inventory.push_back(it->clone());
								res += "\nВы получили с врага предмет " + it->name;
								break;
							}
						}

						for (int j = 0; j < ply->location->enemyDescription.size(); j++) {
							if (ply->location->enemyDescription[j].ID == ply->battleStatus.enemy->ID) {
								ply->location->enemyDescription.erase(ply->location->enemyDescription.begin() + j);
								break;
							}
						}

						for (int i = 0; i < ply->location->enemies.size(); i++) {
							if (ply->location->enemies[i]->battleWith->username == ply->username) {
								ply->location->enemies.erase(ply->location->enemies.begin() + i);
								break;
							}
						}

						ply->battleStatus.enemy->battleWith = nullptr;
						ply->battleStatus.status = NotInBattle;
						break;
					}
					break;
				}
			}
		}

		if (isEnemyDead == false) {
			res += ply->battleStatus.enemy->Attack(ply);
			res += CheckPlayerDeath(ply);
		}

		return res;
	}

	std::string Game::Battle(std::shared_ptr<Player> ply, std::string enemyName) {
		std::string res = "Этого врага нет в комнате";

		for (auto& it : ply->location->enemies) {
			if (it->name == enemyName) {
				if (it->battleWith != nullptr) {
					res = "Игрок " + it->battleWith->username + " уже дерется с этим врагом.";
				}
				else {
					ply->battleStatus.status = InBattle;
					ply->battleStatus.enemy = it;
					it->battleWith = ply;
					res = "Вы вступили в битву с " + it->name;
					break;
				}
			}
		}

		return res;
	}

	std::string Game::CheckPlayerDeath(std::shared_ptr<Player> ply) {
		std::string res = "";

		if (ply->stats.health <= 0) {
			res = "\nВы умерли, ваши вещи остались в комнате [" + std::to_string(ply->location->location.X) + ", "
				+ std::to_string(ply->location->location.Y) + "] на " + std::to_string(ply->location->location.F) + " этаже.";

			ply->battleStatus.enemy->battleWith = nullptr;
			ply->battleStatus.enemy = nullptr;
			ply->battleStatus.status = NotInBattle;

			//inventory drop

			for (int i = ply->inventory.size() - 1; i >= 0; i--) {
				if (ply->inventory[i]->ID == ply->stats.weaponEqID) {
					ply->stats.weaponEqID = 0;
					ply->stats.wepEq = false;
				}

				ply->location->items.push_back(ply->inventory[i]->clone());
				std::string phrase = "";
				for (auto& it : configurator.config.roomDescs) {
					if (ply->location->ID == it.ID) {
						if (it.itemDescs.size() == 1) {
							phrase = std::vformat(std::string_view(it.itemDescs[0]), std::make_format_args(ply->inventory[i]->name));
						}
						else {
							phrase = std::vformat(std::string_view(it.itemDescs[rand() % it.itemDescs.size()]), std::make_format_args(ply->inventory[i]->name));
						}
					}
				}
				ply->location->itemDescription.push_back(RoomDescElement(ply->inventory[i]->ID, phrase));
				ply->inventory.erase(ply->inventory.begin() + i);
			}

			//stats reset

			ply->stats.health = 100;
			ply->stats.armor = 0;
			ply->stats.level = 1;
			ply->stats.weaponSkill = 0;
			ply->stats.inventoryMaxSize = configurator.config.maxInventorySize;

			for (auto& it : gamedata.floors) {
				if (it->level == 1) {
					ply->floor = it;
					ply->location = it->rooms[rand() % it->rooms.size()];
				}
			}
		}

		return res;
	}

	std::string Game::Equip(std::shared_ptr<Player> ply, std::string wepName) {
		std::string res = "Не найден предмет";

		for (auto& it : ply->inventory) {
			if (it->name == wepName) {
				Weapon* wep = dynamic_cast<Weapon*>(it.get());
				if (wep) {
					res = wep->equip(ply);
					break;
				}
				else {
					res = "Это не оружие, вы не можете его экипировать.";
				}
			}
		}

		return res;
	}

	void Game::LoadConfig() {
		configurator.LoadConfig();
		//add commands

		CommandHandler& commandhandler = CommandHandler::Instance();

		commandhandler.add("карта", [this](std::shared_ptr<Player> ply) {return this->Map(ply); }, 0, false);
		commandhandler.add("я", [this](std::shared_ptr<Player> ply) {return this->Me(ply); }, 0, true);
		commandhandler.add("осмотреться", [this](std::shared_ptr<Player> ply) {return this->Look(ply);}, 0, false);

		commandhandler.add("идти", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Move(ply,arg); }, 1, false);
		commandhandler.add("создать", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Craft(ply, arg); }, 1, false);
		commandhandler.add("выбросить", [this](std::shared_ptr<Player> ply, std::string arg) {return this->DropItem(ply, arg); }, 1, false);

		commandhandler.add("поднять", [this](std::shared_ptr<Player> ply, std::string arg) {return this->PickupItem(ply, arg); }, 1, false);
		commandhandler.add("инвентарь", [this](std::shared_ptr<Player> ply) {return this->Inventory(ply); }, 0, true);
		commandhandler.add("вверх", [this](std::shared_ptr<Player> ply) {return this->ChangeFloor(ply, 1); }, 0, false);
		commandhandler.add("вниз", [this](std::shared_ptr<Player> ply) {return this->ChangeFloor(ply, 0); }, 0, false);

		commandhandler.add("осмотреть", [this](std::shared_ptr<Player> ply, std::string arg) {return this->LookItem(ply, arg); }, 1, false);
		commandhandler.add("рецепты", [this](std::shared_ptr<Player> ply) {return this->EnableCrafts(ply); }, 0, false);
		commandhandler.add("использовать", [this](std::shared_ptr<Player> ply, std::string arg) {return this->UseItem(ply, arg); }, 1, true);

		commandhandler.add("атаковать", [this](std::shared_ptr<Player> ply) {return this->Attack(ply); }, 0, true);
		commandhandler.add("битва", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Battle(ply, arg); }, 1, false);
		commandhandler.add("экипировать", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Equip(ply, arg); }, 1, true);
	}

	std::string Game::ParseCommand(std::shared_ptr<Player> ply, std::string& command) {
		std::lock_guard<std::mutex> lock(game_mutex);
		CommandHandler& commandhandler = CommandHandler::Instance();

		std::cout << "Запрос от игрока под ником " << ply->username << "\n";
		bool inb = false;
		if (ply->battleStatus.status == InBattle) { inb = true; }
		std::string res = "";
		res = commandhandler.handle(ply, command, inb);
		return res;
	}
}