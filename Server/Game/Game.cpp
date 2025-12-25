#include "Game.h"

namespace Gigahrush {
	void Configurator::LoadMapSize() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json SizeConfig = js.readFile("Config/MainConfig.json");
		config.mapSize.FloorCount = SizeConfig["FloorCount"];
		config.mapSize.X = SizeConfig["FloorSizeX"];
		config.mapSize.Y = SizeConfig["FloorSizeY"];
		config.maxInventorySize = SizeConfig["MaxInventory"];
		config.maxRoomItems = SizeConfig["MaxRoomItems"];

		std::cout << "=== Main config loaded ===" << std::endl;
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
					ItemsConfig[i]["canSpawn"]);
			}
			else if (ItemsConfig[i]["type"] == 2) {
				item = std::make_unique<Food>(
					ItemsConfig[i]["id"],
					ItemsConfig[i]["name"],
					ItemsConfig[i]["description"],
					ItemsConfig[i]["useDescription"],
					ItemsConfig[i]["canSpawn"]);
			}
			else if (ItemsConfig[i]["type"] == 3) {
				item = std::make_unique<HealingItem>(
					ItemsConfig[i]["id"],
					ItemsConfig[i]["name"],
					ItemsConfig[i]["description"],
					ItemsConfig[i]["useDescription"],
					ItemsConfig[i]["canSpawn"]);
			}

			config.items.push_back(std::move(item));
			config.itemSpawnChances.push_back(SpawnChance(ItemsConfig[i]["id"], ItemsConfig[i]["spawnChance"]));
		}

		std::cout << "=== Items loaded ===" << std::endl;
		std::cout << "Items count: " << config.items.size() << std::endl;

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

			config.enemies.push_back(std::make_unique<Enemy>(
				EnemiesConfig[i]["id"],
				EnemiesConfig[i]["name"],
				EnemiesConfig[i]["description"],
				EnemiesConfig[i]["replics"],
				EnemiesConfig[i]["health"],
				EnemiesConfig[i]["attack"], 
				std::move(loot)
			));
			config.enemySpawnChances.push_back(SpawnChance(EnemiesConfig[i]["id"], EnemiesConfig[i]["spawnChance"]));
		}

		std::cout << "=== Enemies loaded ===" << std::endl;
		std::cout << "Enemies count: " << config.enemies.size() << std::endl;
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
				std::vector<std::unique_ptr<Enemy>>{},
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
		std::cout << "=== Rooms loaded ===" << std::endl;
		std::cout << "Rooms count: " << config.rooms.size() << std::endl;
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

		std::cout << "=== Crafts loaded ===" << std::endl;
		std::cout << "Crafts count: " << config.crafts.size() << std::endl << std::endl;
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
		std::cout << "=== Starting loading config ===" << std::endl << std::endl;

		LoadMapSize();
		LoadItems();
		LoadEnemies();
		LoadRooms();
		LoadCrafts();

		config.configLoaded = true;
		std::cout << "Config loaded!\n\n";
		return true;
		ShowAllConfig();
	}

	Game& Game::Instance() {
		static Game g;
		return g;
	}

	bool Game::changeDir(std::vector<std::vector<int>>& mask, int& X, int& Y, int& randDir) {
		randDir = (rand() % 4) + 1;

		if (X >= 0 && X < mask.size() && Y >= 0 && Y < mask[0].size()) {
			if (mask[X][Y] == 0) { return true; }
		}

		int tempX = X;
		int tempY = Y;

		switch (randDir) {
			case 1:
				tempY += 1;
				break;
			case 2:
				tempY -= 1;
				break;
			case 3:
				tempX -= 1;
				break;
			case 4:
				tempX += 1;
				break;
		}

		if (tempX >= 0 && tempX <= mask.size() && tempY >= 0 && tempY <= mask[0].size()) {
			X = tempX;
			Y = tempY;

			return true;
		}
		else {
			return false;
		}

	}

	void Game::GenerateBranchMask(std::vector<std::vector<int>>& mask, int currentX, int currentY, int remainingSteps, int currentDir) {
		if (remainingSteps <= 0 || currentX < 0 || currentX >= mask.size() || currentY < 0 || currentY >= mask[0].size()) {
			return;
		}

		if (mask[currentX][currentY] == 1) { return; }
		mask[currentX][currentY] = 1;

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

			if (newX >= 0 && newX < mask.size() && newY >= 0 && newY < mask[0].size()) {
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

					if (newX >= 0 && newX < mask.size() && newY >= 0 && newY < mask[0].size()) {
						GenerateFloorMask(mask, newX, newY, remainingSteps - 1, randDir);
					}
				}
			}
		}
	}

	void Game::GenerateFloorMask(std::vector<std::vector<int>>& mask, int currentX, int currentY, int remainingSteps, int currentDir) {
		if (remainingSteps <= 0 || currentX < 0 || currentX >= mask.size() || currentY < 0 || currentY >= mask[0].size()) {
			return;
		}

		if (mask[currentX][currentY] == 1) { return; }
		mask[currentX][currentY] = 1;

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

			if (newX >= 0 && newX <= mask.size() && newY >= 0 && newY <= mask[0].size()) {
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

					if (newX >= 0 && newX <= mask.size() && newY >= 0 && newY <= mask[0].size()) {
						GenerateFloorMask(mask, newX, newY, remainingSteps - 1, randDir);
					}
				}
			}
		}
	}

	void Game::PrintRoomInfo(std::unique_ptr<Room>& rm) {
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

		std::cout << "--- items room descs ---\n\n";

		for (auto& t : rm->itemDescription) {
			std::cout << "Room description: " << t.desc << "\n";
		}

		std::cout << "Enemies:\n\n";

		for (auto& it : rm->enemies) {
			std::cout << "Name: " << it->name << "\n";
		}

		std::cout << "--- enemy room descs ---\n\n";

		for (auto& t : rm->enemyDescription) {
			std::cout << "Room description: " << t.desc << "\n";
		}
	}

	void Game::GenerateItemsAndEnemies(std::unique_ptr<Room>& rm) {
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
					phrase = std::vformat(std::string_view(it.itemDescs[rand() % it.itemDescs.size()]), std::make_format_args(itName));
					//phrase = it.itemDescs[rand() % it.itemDescs.size()];
					//sprintf(&phrase[0], itName.c_str());
				}
			}

			itemDescription.push_back(RoomDescElement(itemId, phrase));
		}

		//Gen enemy
		std::vector<RoomDescElement> enemyDescription;
		std::vector<std::unique_ptr<Enemy>> enemies;

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
						phrase = std::vformat(std::string_view(it.enemiesDescs[rand() % it.enemiesDescs.size()]), std::make_format_args(enName));
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
		rm->enemies = std::move(enemies);
	}

	std::unique_ptr<Room> Game::GenerateRoom(Location loc, bool isExit) {
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
		std::unique_ptr<Room> room;

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

			std::unique_ptr<Floor> flr = std::make_unique<Floor>(
				0,
				std::vector<std::unique_ptr<Room>>{},
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

			flr->floorMask.resize(configurator.config.mapSize.X, std::vector<int>(configurator.config.mapSize.Y, 0));

			int randStartX = configurator.config.mapSize.X/2;
			int randStartY = configurator.config.mapSize.Y/2;

			flr->exitCoordinates = Location(randStartX, randStartY, i);

			int remainingSteps = configurator.config.mapSize.X;

			GenerateFloorMask(flr->floorMask, randStartX, randStartY, remainingSteps, (rand()%4)+1);

			//flr->floorMask = mask;

			//Print mask

			for (int y = 0; y < flr->floorMask.size(); y++) {
				for (int x = 0; x < flr->floorMask[0].size(); x++) {
					if (flr->floorMask[x][y] == 1) {
						bool isex;
						Location loc(x, y, i);

						if (loc == flr->exitCoordinates) { isex = true; }
						else { isex = false; }

						flr->rooms.push_back(GenerateRoom(loc,isex));
						//std::cout << "#";
					}
					else {
						//std::cout << " ";
					}
				}
				//std::cout << "\n";
			}
			gamedata.floors.push_back(std::move(flr));
		}
	}

	bool Game::GenerateGame() {
		gamedata = GameData();
		std::cout << "=== GENERATING GAME ===\n\n";
		if (configurator.config.configLoaded == true) { //&& isGenerated == false
			auto start = std::chrono::high_resolution_clock::now();
			GenerateFloors();
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			std::cout << "Game generated in " << duration << ", server now can be started!\n";
			isGenerated = true;
			return true;
		}
		else {
			/*
			if (isGenerated) {
				std::cout << "Can't generate game, already generated. Please type 'reset' for reset game.\n";
				return false;
			}
			*/

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

			std::cout << "=== GAME INFO ===\n\n";
			std::cout << "Floors count: " << gamedata.floors.size() << "\n";
			std::cout << "Rooms count: " << rooms << "\n";
			std::cout << "Items count: " << items << "\n";
			std::cout << "Enemies count: " << enemies << "\n";
		}
		else {
			std::cout << "No info. Game is not generated or loaded\n";
		}
	}

	std::string Game::ParseCommand() {
		std::cout << "Парсилка команды\n";
		return "Елда!";
	}
}