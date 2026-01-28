#include "Game.h"
#include "../utils.h"
#include "../Server.h"

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

		config.samosborconfig.minInterval = SizeConfig["Samosbor"]["minInterval"];
		config.samosborconfig.maxInterval = SizeConfig["Samosbor"]["maxInterval"];
		config.samosborconfig.minDuration = SizeConfig["Samosbor"]["minDuration"];
		config.samosborconfig.maxDuration = SizeConfig["Samosbor"]["maxDuration"];
		config.samosborconfig.liveRoomsIDs = SizeConfig["Samosbor"]["liveRoomsIDs"].get<std::vector<int>>();

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
			if (RoomsConfig[i]["type"] == 1) {
				config.rooms.push_back(std::make_unique<Room>(
					RoomsConfig[i]["id"],
					RoomsConfig[i]["name"],
					std::string{ RoomsConfig[i]["description"] },
					std::vector<RoomDescElement>{},
					std::vector<RoomDescElement>{},
					std::vector<std::unique_ptr<Item>>{},
					std::vector<std::shared_ptr<Enemy>>{},
					RoomsConfig[i]["isExit"],
					Location()
				));
			}
			else if (RoomsConfig[i]["type"] == 2) {
				config.rooms.push_back(std::make_unique<ExitRoom>(
					RoomsConfig[i]["id"],
					RoomsConfig[i]["name"],
					std::string{ RoomsConfig[i]["description"] },
					std::vector<RoomDescElement>{},
					std::vector<RoomDescElement>{},
					std::vector<std::unique_ptr<Item>>{},
					std::vector<std::shared_ptr<Enemy>>{},
					RoomsConfig[i]["isExit"],
					Location(),
					false,
					RoomsConfig[i]["repairRecipe"]
				));

			}

			config.roomDescs.push_back(RoomDesc(
				RoomsConfig[i]["id"],
				RoomsConfig[i]["itemDescs"],
				RoomsConfig[i]["enemiesDescs"]
			));

			if (RoomsConfig[i]["type"] == 2) {
				config.exitRoomSpawnChances.push_back(SpawnChance(RoomsConfig[i]["id"], RoomsConfig[i]["spawnChance"]));
			}
			else {
				config.roomSpawnChances.push_back(SpawnChance(RoomsConfig[i]["id"], RoomsConfig[i]["spawnChance"]));
			}

			config.roomUniqueItems.push_back(RoomUniqueItems(RoomsConfig[i]["id"], RoomsConfig[i]["unItems"]));
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

	Game::Game(): isGenerated(false), isReseted(true), samosborGoing(false), srv(nullptr) {}
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
		std::vector<int> uniqItems;

		int itemCount = (rand() % configurator.config.maxRoomItems) + 1;

		for (auto& d : configurator.config.roomUniqueItems) {
			if (d.ID == rm->ID) {
				uniqItems = d.itemsID;
				break;
			}
		}

		if (uniqItems.size() != 0) {
			for (int i = 0; i < itemCount; i++) {

				if (rand() % 100 <= 30) { continue; }

				int itemId = 0;

				//Random algoritm
				int Weight = 0;

				for (auto& ch : configurator.config.itemSpawnChances) {
					if (std::find(uniqItems.begin(), uniqItems.end(), ch.ID) != uniqItems.end()) {
						Weight += (ch.chance * 100);
					}
				}

				int choose = rand() % Weight;

				Weight = 0;

				for (auto& ch : configurator.config.itemSpawnChances) {
					if (std::find(uniqItems.begin(), uniqItems.end(), ch.ID) != uniqItems.end()) {
						Weight += (ch.chance * 100);
						if (Weight >= choose) {
							itemId = ch.ID;
							break;
						}
					}
				}

				std::string itName = "";
				//End random
				for (auto& it : configurator.config.items) {
					if (it->ID == itemId) {
						itName = it->name;
						items.push_back(it->clone());
						break;
					}
				}

				std::string phrase = "";

				for (auto& it : configurator.config.roomDescs) {
					if (rm->ID == it.ID) {
						if (it.itemDescs.size() == 1) {
							phrase = std::vformat(std::string_view(it.itemDescs[0]), std::make_format_args(itName));
						}
						else {
							phrase = std::vformat(std::string_view(it.itemDescs[rand() % (it.itemDescs.size()-1)]), std::make_format_args(itName));
						}

						//phrase = it.itemDescs[rand() % it.itemDescs.size()];
						//sprintf(&phrase[0], itName.c_str());
					}
				}

				itemDescription.push_back(RoomDescElement(itemId, phrase));
			}
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
							phrase = std::vformat(std::string_view(it.enemiesDescs[rand() % (it.enemiesDescs.size()-1)]), std::make_format_args(enName));
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

	void Game::AddRepairItemsToFloor(std::shared_ptr<Floor> flr) {
		for (auto& it : flr->rooms) {
			if (it->isExit == true) {
				ExitRoom* rm = dynamic_cast<ExitRoom*>(it.get());
				if (rm != nullptr) {
					std::vector<int> repRec = rm->repairRecipe;
					for (auto xd : repRec) {
						for (auto& item : configurator.config.items) {
							if (item->ID == xd) {
								std::shared_ptr<Room> randRoom;
								if (flr->rooms.size() == 1) {
									randRoom = flr->rooms[0];
								}
								else {
									randRoom = flr->rooms[rand() % (flr->rooms.size() - 1)];
								}

								std::string phrase = "";

								for (auto& it : configurator.config.roomDescs) {
									if (randRoom->ID == it.ID) {
										if (it.itemDescs.size() == 1) {
											phrase = std::vformat(std::string_view(it.itemDescs[0]), std::make_format_args(item->name));
										}
										else {
											phrase = std::vformat(std::string_view(it.itemDescs[rand() % (it.itemDescs.size()-1)]), std::make_format_args(item->name));
										}
									}
								}

								randRoom->itemDescription.push_back(RoomDescElement(item->ID, phrase));
								randRoom->items.push_back(item->clone());
								//std::cout << "Add repair " << item->name << " on floor " << std::to_string(flr->level) << " [" << std::to_string(randRoom->location.X) << ", " << std::to_string(randRoom->location.Y) << "]\n";
							}
						}
					}
				}
			}
		}
	}

	std::shared_ptr<Room> Game::GenerateRoom(Location loc, bool isExit) {
		int RoomID = 0;

		//Random algoritm
		int Weight = 0;
		std::shared_ptr<Room> room;

		if (!isExit) {
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

			for (auto& rm : configurator.config.rooms) {
				if (rm->ID == RoomID) {
					room = rm->clone();
				}
			}

			room->isExit = isExit;
			room->location = loc;

			GenerateItemsAndEnemies(room);
			//PrintRoomInfo(room);
		}
		else {
			for (auto ch : configurator.config.exitRoomSpawnChances) {
				Weight += (ch.chance * 100);
			}

			int choose = rand() % Weight;

			Weight = 0;

			for (auto ch : configurator.config.exitRoomSpawnChances) {
				Weight += (ch.chance * 100);
				if (Weight >= choose) {
					RoomID = ch.ID;
					break;
				}
			}
			//End random

			for (auto& rm : configurator.config.rooms) {
				if (rm->ID == RoomID) {
					room = rm->clone();
				}
			}

			room->isExit = isExit;
			room->location = loc;

			ExitRoom* rmm = dynamic_cast<ExitRoom*>(room.get());

			if (rmm != nullptr) { 
				rmm->isBroken = true;
				


				/*if (rand() % 100 < 5) {
					rmm->isBroken = true;
				}
				else {
					rmm->isBroken = false;
				}*/
			}

			GenerateItemsAndEnemies(room);
			//PrintRoomInfo(room);
		}

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

			AddRepairItemsToFloor(flr);
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
			isReseted = false;
			isGenerated = true;
			srv->stopSamosbor(false);
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

			std::cout << "=== Players on server ===\n";
			size_t c = 1;
			for (auto& it : gamedata.players) {
				std::cout << std::to_string(c) + ". " + it->username << " on floor " << std::to_string(it->location->location.F) << "\n";
				++c;
			}
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
		ply->stats = PlayerStats(100, 0, 1, 100, 0, configurator.config.maxInventorySize, false, 0, 0, false); //health,armor,level,toup,cur, max inv size, is weapon eq, weapon id, weaponskills

		for (auto& it : gamedata.floors) {
			if (it->level == 1) {
				ply->floor = it;
				if (it->rooms.size() == 1) {
					ply->location = it->rooms[0];
				}
				else {
					ply->location = it->rooms[rand() % (it->rooms.size() - 1)];
				}
				//std::cout << ply->location->description;
			}
		}

		gamedata.players.push_back(ply);
		return ply;
	}

	std::string Game::Me(std::shared_ptr<Gigahrush::Player> ply) {
		/*
		std::string res = "Имя: " + ply->username + "\n" +
			"Уровень: " + std::to_string(ply->stats.level) + "\n" +
			"Текущий опыт: " + std::to_string(ply->stats.currentExp) + "\n" +
			"Опыта до следующего уровня: " + std::to_string(ply->stats.expTolevelUp) + "\n" +
			"Вместимость инвентаря: " + std::to_string(ply->stats.inventoryMaxSize) + "\n" +
			"Здоровье: " + std::to_string(ply->stats.health) + "\n" +
			"Броня: " + std::to_string(ply->stats.armor) + "\n" +
			"Навык владения оружием: " + std::to_string(ply->stats.weaponSkill);

		if (ply->stats.wepEq) {
			for (auto& it : ply->inventory) {
				if (it->ID == ply->stats.weaponEqID) {
					res += "\nТекущее оружие: " + it->name;
					break;
				}
			}
		}

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "Me";

		res["content"]["username"] = ply->username;
		res["content"]["level"] = ply->stats.level;
		res["content"]["exp"] = ply->stats.currentExp;
		res["content"]["expToLU"] = ply->stats.expTolevelUp;
		res["content"]["maxInventory"] = ply->stats.inventoryMaxSize;
		res["content"]["health"] = ply->stats.health;
		res["content"]["armor"] = ply->stats.armor;
		res["content"]["weaponSkill"] = ply->stats.weaponSkill;
		res["content"]["currentWeapon"] = "";

		if (ply->stats.wepEq) {
			for (auto& it : ply->inventory) {
				if (it->ID == ply->stats.weaponEqID) {
					res["content"]["currentWeapon"] = it->name;
					break;
				}
			}
		}

		return res.dump();
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

		return toJson(res, "MAP");
	}

	std::string Game::Look(std::shared_ptr<Gigahrush::Player> ply) {
		/*
		std::string res = "\n" + ply->location->name + ": Этаж " + std::to_string(ply->floor->level) + "\n" +
			"Координаты: [" + std::to_string(ply->location->location.X) + ", " + std::to_string(ply->location->location.Y) + "]\n\n===ОПИСАНИЕ===\n\n"+
			ply->location->description;

		size_t c = 1;

		if (ply->location->itemDescription.size() != 0) { res += "\n\n===ПРЕДМЕТЫ===\n"; }

		for (auto& it : ply->location->itemDescription) {
			res += "\n" + std::to_string(c) + ". " + it.desc;
			++c;
		}

		if (ply->location->enemyDescription.size() != 0) { res += "\n\n===ВРАГИ===\n"; }
		c = 1;

		for (auto& it : ply->location->enemyDescription) {
			res += "\n" + std::to_string(c) + ". " + it.desc;
			++c;
		}
		std::string temp = "";

		std::vector<std::vector<int>>& mask = ply->floor->floorMask;
		int plyX = ply->location->location.X;
		int plyY = ply->location->location.Y;

		if ((plyY - 1 >= 0 && plyY - 1 < mask.size() && plyX >= 0 && plyX < mask[0].size()) && mask[plyY-1][plyX] == 1) { temp += "север "; }
		if ((plyY + 1 >= 0 && plyY + 1 < mask.size() && plyX >= 0 && plyX < mask[0].size()) && mask[plyY+1][plyX] == 1) { temp += "юг "; }
		if ((plyY >= 0 && plyY < mask.size() && plyX - 1 >= 0 && plyX - 1 < mask[0].size()) && mask[plyY][plyX-1] == 1) { temp += "запад "; }
		if ((plyY >= 0 && plyY < mask.size() && plyX + 1 >= 0 && plyX + 1 < mask[0].size()) && mask[plyY][plyX+1] == 1) { temp += "восток "; }

		if(temp != "") { res += "\n\nВы можете пойти на " + temp; }

		if (ply->location->isExit == true) {
			ExitRoom* rm = dynamic_cast<ExitRoom*>(ply->location.get());

			if (rm != nullptr && rm->isBroken == false) {
				if (ply->floor->canGoUp == true) {
					res += "\n\nИз этой локации вы можете подняться на этаж выше";
				}
				if (ply->floor->canGoDown == true) {
					res += "\n\nИз этой локации вы можете опуститься на этаж ниже";
				}
			}
			else {
				res += "\n\nНа локации сломан 
				, вы не можете перейти на другой этаж пока не почините лифт";
			}
		}
		*/

		nlohmann::json res;

		//ANSWER Type

		res["type"] = "ANSWER";
		res["content"]["type"] = "Look";
		res["content"]["coordinates"]["x"] = ply->location->location.X;
		res["content"]["coordinates"]["y"] = ply->location->location.Y;
		res["content"]["coordinates"]["floor"] = ply->floor->level;
		res["content"]["coordinates"]["canGoUp"] = false;
		res["content"]["coordinates"]["canGoDown"] = false;
		res["content"]["coordinates"]["isExitBroken"] = false;
		res["content"]["locationName"] = ply->location->name;
		res["content"]["locationDescription"] = ply->location->description;

		res["content"]["items"] = nlohmann::json::array();

		if (ply->location->itemDescription.size() != 0) {
			for (auto& it : ply->location->itemDescription) {
				std::string name = "";

				for (auto& itt : configurator.config.items) {
					if (itt->ID == it.ID) { name = itt->name; break; }
				}

				res["content"]["items"].push_back({
					{"name",name},
					{"description",it.desc}
				});
			}
		}

		res["content"]["enemies"] = nlohmann::json::array();

		if (ply->location->enemyDescription.size() != 0) {
			for (auto& it : ply->location->enemyDescription) {
				std::string name = "";

				for (auto& itt : configurator.config.enemies) {
					if (itt->ID == it.ID) { name = itt->name; break; }
				}

				res["content"]["enemies"].push_back({
					{"name",name},
					{"description",it.desc}
				});
			}
		}

		res["content"]["sides"] = nlohmann::json::array();

		std::vector<std::vector<int>>& mask = ply->floor->floorMask;
		int plyX = ply->location->location.X;
		int plyY = ply->location->location.Y;

		if ((plyY - 1 >= 0 && plyY - 1 < mask.size() && plyX >= 0 && plyX < mask[0].size()) && mask[plyY - 1][plyX] == 1) { res["content"]["sides"].push_back("север"); }
		if ((plyY + 1 >= 0 && plyY + 1 < mask.size() && plyX >= 0 && plyX < mask[0].size()) && mask[plyY + 1][plyX] == 1) { res["content"]["sides"].push_back("юг"); }
		if ((plyY >= 0 && plyY < mask.size() && plyX - 1 >= 0 && plyX - 1 < mask[0].size()) && mask[plyY][plyX - 1] == 1) { res["content"]["sides"].push_back("запад"); }
		if ((plyY >= 0 && plyY < mask.size() && plyX + 1 >= 0 && plyX + 1 < mask[0].size()) && mask[plyY][plyX + 1] == 1) { res["content"]["sides"].push_back("восток"); }

		if (ply->location->isExit == true) {
			ExitRoom* rm = dynamic_cast<ExitRoom*>(ply->location.get());

			if (rm != nullptr && rm->isBroken == false) {
				if (ply->floor->canGoUp == true) {
					res["content"]["coordinates"]["canGoUp"] = true;
				}
				if (ply->floor->canGoDown == true) {
					res["content"]["coordinates"]["canGoDown"] = true;
				}
			}
			else {
				res["content"]["coordinates"]["isExitBroken"] = true;
			}
		}

		return res.dump();
	}

	std::string Game::Move(std::shared_ptr<Gigahrush::Player> ply, std::string side) {
		/*
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
		*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "Move";
		res["content"]["canMove"] = true;
		res["content"]["moved"] = false;
		res["content"]["unknownSide"] = false;
		res["content"]["look"] = nlohmann::json::object();

		std::vector<std::vector<int>> mask = ply->floor->floorMask;

		int posX = ply->location->location.X;
		int posY = ply->location->location.Y;

		if (side == "север") { posY -= 1; }
		else if (side == "юг") { posY += 1; }
		else if (side == "запад") { posX -= 1; }
		else if (side == "восток") { posX += 1; }
		else { 
			res["content"]["unknownSide"] = true;
			return res.dump();
		}

		if (posY < 0 || posY >= mask.size() || posX < 0 || posX >= mask[0].size()) { 
			res["content"]["canMove"] = false;
			return res.dump();
		}
		if (mask[posY][posX] != 1) { 
			res["content"]["canMove"] = false;
			return res.dump();
		}

		for (auto& it : ply->floor->rooms) {
			if (it->location.X == posX && it->location.Y == posY) {
				ply->location = it;
				res["content"]["moved"] = true;
				res["content"]["look"] = nlohmann::json::parse(Look(ply));
				return res.dump();
			}
		}

		res["content"]["canMove"] = false;
		return res.dump();
	}

	std::string Game::Craft(std::shared_ptr<Gigahrush::Player> ply, std::string item) {
		/*
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
								res += "\nВы потеряли " + ply->inventory[i]->name;
								ply->inventory.erase(ply->inventory.begin() + i);
								--delRemain;
							}
						}
					}
					res += "\nВы успешно скрафтили предмет - " + itemToCraft->name;
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

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "Craft";

		res["content"]["noItemFound"] = false;
		res["content"]["noResources"] = false;
		res["content"]["noCraftFound"] = false;

		res["content"]["lostedItems"] = nlohmann::json::array(); 
		res["content"]["craftedItem"] = "";

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
								res["content"]["lostedItems"].push_back(ply->inventory[i]->name);
								ply->inventory.erase(ply->inventory.begin() + i);
								--delRemain;
							}
						}
					}
					res["content"]["craftedItem"] = itemToCraft->name;
					ply->inventory.push_back(std::move(itemToCraft));
				}
				else {
					res["content"]["noResources"] = true;
				}
			}
			else {
				res["content"]["noCraftFound"] = true;
			}
		}
		else {
			res["content"]["noItemFound"] = true;
		}

		return res.dump(); 
	}

	std::string Game::DropItem(std::shared_ptr<Gigahrush::Player> ply, std::string item) {
		/*
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
		return res;*/
		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "Drop";
		res["content"]["dropped"] = "";
		res["content"]["itemFound"] = false;

		for (int i = 0; i < ply->inventory.size(); i++) {
			if (ply->inventory[i]->name == item) {
				if (ply->inventory[i]->ID == ply->stats.weaponEqID) {
					ply->stats.weaponEqID = 0;
					ply->stats.wepEq = false;
				}

				ply->location->items.push_back(ply->inventory[i]->clone());
				res["content"]["dropped"] = ply->inventory[i]->name;
				res["content"]["itemFound"] = true;

				std::string phrase = "";
				for (auto& it : configurator.config.roomDescs) {
					if (ply->location->ID == it.ID) {
						if (it.itemDescs.size() == 1) {
							phrase = std::vformat(std::string_view(it.itemDescs[0]), std::make_format_args(item));
						}
						else {
							phrase = std::vformat(std::string_view(it.itemDescs[rand() % (it.itemDescs.size()-1)]), std::make_format_args(item));
						}
					}
				}
				ply->location->itemDescription.push_back(RoomDescElement(ply->inventory[i]->ID, phrase));
				ply->inventory.erase(ply->inventory.begin() + i);
				break;
			}
		}
		return res.dump();
	}

	std::string Game::PickupItem(std::shared_ptr<Gigahrush::Player> ply, std::string item) {
		/*
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
		*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "Pickup";
		res["content"]["canPickup"] = true;
		res["content"]["item"] = "";
		res["content"]["pickuped"] = false;
		res["content"]["isInventoryFull"] = false;

		if (ply->location->enemies.size() != 0) {
			res["content"]["canPickup"] = false;
			return res.dump();
		}

		bool isFound = false;
		for (int i = 0; i < ply->location->items.size(); i++) {
			if (ply->location->items[i]->name == item) {
				for (int j = 0; j < ply->location->itemDescription.size(); j++) {
					if (ply->location->itemDescription[j].ID == ply->location->items[i]->ID) {
						if (ply->inventory.size() < ply->stats.inventoryMaxSize) {
							ply->inventory.push_back(ply->location->items[i]->clone());
							res["content"]["item"] = ply->location->items[i]->name;
							res["content"]["pickuped"] = true;
							ply->location->items.erase(ply->location->items.begin() + i);
							ply->location->itemDescription.erase(ply->location->itemDescription.begin() + j);
							isFound = true;
							break;
						}
						else {
							res["content"]["isInventoryFull"] = true;
							break;
						}
					}
				}
			}
			if (isFound) {
				break;
			}
		}

		return res.dump();
	}

	std::string Game::Inventory(std::shared_ptr<Player> ply) {
		/*
		std::string res = "Ваш инвентарь:";
		size_t i = 1;

		for (auto& it : ply->inventory) {
			res += "\n" + std::to_string(i) + ". " + it->name;
			++i;
		}

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "Inventory";
		res["content"]["items"] = nlohmann::json::array();

		for (auto& it : ply->inventory) {
			res["content"]["items"].push_back(it->name);
		}

		return res.dump();
	}

	std::string Game::ChangeFloor(std::shared_ptr<Player> ply, int dir) {
		/*
		//1 - up 0 - down
		std::string res = "";
		std::shared_ptr<Floor> flrToChange = nullptr;
		std::shared_ptr<Room> roomToChange = nullptr;

		bool canChange = false;
		
		ExitRoom* rmm = dynamic_cast<ExitRoom*>(ply->location.get());

		if (rmm != nullptr) {
			if (rand() % 100 < 5) {
				rmm->isBroken = true;
				return "Вы сломали лифт, вы не сможете сменить этаж пока не почините его";
			}
		}

		if (rmm != nullptr && rmm->isBroken == true) {
			return "Вы не можете сменить этаж";
		}

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

		return res;*/

		//1 - up 0 - down
		
		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "ChangeFloor";

		res["content"]["isElevatorBroken"] = false;
		res["content"]["isPlayerBrokeElevator"] = false;
		res["content"]["canGoUp"] = true;
		res["content"]["canGoDown"] = true;
		res["content"]["moveType"] = dir; //up down 1 0 
		res["content"]["changed"] = false;
		res["content"]["canChange"] = true;
		res["content"]["changedFloor"] = 0;

		std::shared_ptr<Floor> flrToChange = nullptr;
		std::shared_ptr<Room> roomToChange = nullptr;

		bool canChange = false;

		ExitRoom* rmm = dynamic_cast<ExitRoom*>(ply->location.get());

		/*if (rmm != nullptr) {
			if (rand() % 100 < 5) {
				rmm->isBroken = true;
				res["content"]["isPlayerBrokeElevator"] = true;
				return res.dump();
			}
		}*/

		if (rmm != nullptr && rmm->isBroken == true) {
			res["content"]["isElevatorBroken"] = true;
			return res.dump();
		}

		if (ply->location->isExit == true) {
			if (dir == 0)
				if (ply->floor->canGoDown == true) {
					for (auto& it : gamedata.floors) {
						if (it->level == ply->floor->level - 1) {
							flrToChange = it;
							for (auto& at : it->rooms) {
								if (at->location == it->exitCoordinates) {
									roomToChange = at;
									canChange = true;
									res["content"]["changedFloor"] = it->level;
									res["content"]["changed"] = true;
								}
							}
						}
					}
				}
				else {
					res["content"]["canGoDown"] = false;
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
									res["content"]["changedFloor"] = it->level;
									res["content"]["changed"] = true;
								}
							}
						}
					}
				}
				else {
					res["content"]["canGoDown"] = false;
				}
			}
		}
		else {
			res["content"]["canChange"] = false;
		}

		if (canChange) {
			ply->location = roomToChange;
			ply->floor = flrToChange;
		}

		return res.dump();
	}

	std::string Game::LookItem(std::shared_ptr<Player> ply, std::string item) {
		/*
		std::string res = "У вас не такого предмета или такого врага нет в комнате";

		for (auto& it : ply->inventory) {
			if (it->name == item) {
				res = it->getDescription();
				break;
			}
		}

		for (auto& it : ply->location->enemies) {
			if (it->name == item) {
				res = it->name + ": " + it->description + "\nЗдоровье: " + std::to_string(it->health) + "\nУрон: " + std::to_string(it->attack);
				break;
			}
		}

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "LookItem";
		res["content"]["found"] = false;
		res["content"]["object"] = nlohmann::json::object();

		for (auto& it : ply->inventory) {
			if (it->name == item) {
				res["content"]["found"] = true;
				res["content"]["object"] = nlohmann::json::parse(it->getDescription());
				break;
			}
		}

		for (auto& it : ply->location->enemies) {
			if (it->name == item) {
				res["content"]["found"] = true;

				nlohmann::json enem;
				enem["type"] = "Enemy";
				enem["name"] = it->name;
				enem["description"] = it->description;
				enem["health"] = it->health;
				enem["damage"] = it->attack;

				res["content"]["object"] = enem;

				break;
			}
		}

		return res.dump(); 
	}

	std::string Game::EnableCrafts(std::shared_ptr<Player> ply) {
		/*
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

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "Recipes";
		res["content"]["enableCrafts"] = nlohmann::json::array();

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
					res["content"]["enableCrafts"].push_back(it->name);
					break;
				}
			}
		}

		return res.dump(); 
	}

	std::string Game::UseItem(std::shared_ptr<Player> ply, std::string item) {
		/*
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

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "UseItem";
		res["content"]["haveItem"] = false;
		res["content"]["enemyStep"] = nlohmann::json::object();
		res["content"]["checkPlayerDeath"] = nlohmann::json::object();
		res["content"]["playerInBattle"] = false;

		res["content"]["item"] = nlohmann::json::object();

		for (int i = 0; i < ply->inventory.size(); i++) {
			if (ply->inventory[i]->name == item) {
				std::pair<std::string, bool> useRes = ply->inventory[i]->use(ply);
				res["content"]["item"] = nlohmann::json::parse(useRes.first);
				res["content"]["haveItem"] = true;
				if (useRes.second == true) {
					ply->inventory.erase(ply->inventory.begin() + i);
				}
				break;
			}
		}

		if (ply->battleStatus.status == InBattle) {
			res["content"]["playerInBattle"] = true;
			res["content"]["enemyStep"] = nlohmann::json::parse(ply->battleStatus.enemy->Attack(ply));
			res["content"]["checkPlayerDeath"] = nlohmann::json::parse(CheckPlayerDeath(ply));
		}

		return res.dump();
	}

	std::string Game::CheckLevelUp(std::shared_ptr<Player> ply) {
		/*
		std::string res = "";

		if (ply->stats.currentExp < ply->stats.expTolevelUp) { return ""; }

		ply->stats.currentExp -= ply->stats.expTolevelUp;
		ply->stats.expTolevelUp *= 1.1;
		ply->stats.level += 1;

		ply->stats.weaponSkill += 1;
		ply->stats.inventoryMaxSize += 1;

		res = "\n\nВы получили новый уровень:  " + std::to_string(ply->stats.level) + "\n" +
			"Опыта до следующего уровня: " + std::to_string(ply->stats.expTolevelUp) + "\n" +
			"Текущий опыт: " + std::to_string(ply->stats.currentExp) + "\n";

		return res;
		*/

		nlohmann::json res;
		res["isLevelUp"] = false;
		res["newLevel"] = 0;
		res["nextLevelExp"] = 0;
		res["currentExp"] = 0;

		if (ply->stats.currentExp < ply->stats.expTolevelUp) { return res.dump(); }
		res["isLevelUp"] = true;
		ply->stats.currentExp -= ply->stats.expTolevelUp;
		ply->stats.expTolevelUp *= 1.1;
		ply->stats.level += 1;

		ply->stats.weaponSkill += 1;
		ply->stats.inventoryMaxSize += 1;

		res["newLevel"] = ply->stats.level;
		res["nextLevelExp"] = ply->stats.expTolevelUp;
		res["currentExp"] = ply->stats.currentExp;

		return res.dump();
	}

	std::string Game::Attack(std::shared_ptr<Player> ply) {
		/*
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
						ply->stats.currentExp += ply->battleStatus.enemy->exp;

						isEnemyDead = true;
						res += "\nВы победили " + ply->battleStatus.enemy->name + " и получили " + std::to_string(ply->battleStatus.enemy->exp) + " опыта.";
						int randItemFromEnemyID = 0;

						res += CheckLevelUp(ply);

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
							if (ply->location->enemies[i]->battleWith != nullptr) {
								if (ply->location->enemies[i]->battleWith->username == ply->username) {
									ply->location->enemies.erase(ply->location->enemies.begin() + i);
									break;
								}
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

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "Attack";
		res["content"]["inBattle"] = true;
		res["content"]["wepEquiped"] = true;

		res["content"]["enemyName"] = "";

		res["content"]["makeDamage"] = 0;
		res["content"]["skillDamage"] = 0;
		res["content"]["enemyRemainHealth"] = 0;

		res["content"]["isEnemyDead"] = false;
		res["content"]["winExp"] = 0;
		res["content"]["itemFromEnemy"] = "";

		res["content"]["enemyStep"] = nlohmann::json::object();
		res["content"]["checkPlayerDeath"] = nlohmann::json::object();
		res["content"]["levelUp"] = nlohmann::json::object();

		res["content"]["pickedUpWinItem"] = false;

		if (ply->battleStatus.status != InBattle) { res["content"]["inBattle"] = false; return res.dump(); }

		bool isEnemyDead = false;

		if (ply->stats.wepEq == false) {
			res["content"]["wepEquiped"] = false;

			if (isEnemyDead == false) {
				res["content"]["enemyStep"] = nlohmann::json::parse(ply->battleStatus.enemy->Attack(ply));
				res["content"]["checkPlayerDeath"] = nlohmann::json::parse(CheckPlayerDeath(ply));
			}

			return res.dump();
		}

		for (auto& it : ply->inventory) {
			if (it->ID == ply->stats.weaponEqID) {
				Weapon* wep = dynamic_cast<Weapon*>(it.get());
				if (wep != nullptr) {
					ply->battleStatus.enemy->health = std::clamp(ply->battleStatus.enemy->health - (wep->damage + ply->stats.weaponSkill), 0, 1000);
					
					res["content"]["enemyName"] = ply->battleStatus.enemy->name;
					res["content"]["makeDamage"] = wep->damage;
					res["content"]["skillDamage"] = ply->stats.weaponSkill;
					res["content"]["enemyRemainHealth"] = ply->battleStatus.enemy->health;

					if (ply->battleStatus.enemy->health <= 0) {
						ply->stats.currentExp += ply->battleStatus.enemy->exp;

						isEnemyDead = true;
						
						res["content"]["isEnemyDead"] = true;
						res["content"]["winExp"] = ply->battleStatus.enemy->exp;

						int randItemFromEnemyID = 0;

						res["content"]["levelUp"] = nlohmann::json::parse(CheckLevelUp(ply));

						if (ply->battleStatus.enemy->loot.size() != 1) {
							randItemFromEnemyID = ply->battleStatus.enemy->loot[rand() % (ply->battleStatus.enemy->loot.size() - 1)]->ID;
						}
						else {
							randItemFromEnemyID = ply->battleStatus.enemy->loot[0]->ID;
						}

						for (auto& it : configurator.config.items) {
							if (it->ID == randItemFromEnemyID) {
								if (ply->inventory.size() < ply->stats.inventoryMaxSize) {
									ply->inventory.push_back(it->clone());
									res["content"]["itemFromEnemy"] = it->name;
									res["content"]["pickedUpWinItem"] = true;
								}
								else {
									res["content"]["pickedUpWinItem"] = false;
								}
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
							if (ply->location->enemies[i]->battleWith != nullptr) {
								if (ply->location->enemies[i]->battleWith->username == ply->username) {
									ply->location->enemies.erase(ply->location->enemies.begin() + i);
									break;
								}
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
			res["content"]["enemyStep"] = nlohmann::json::parse(ply->battleStatus.enemy->Attack(ply));
			res["content"]["checkPlayerDeath"] = nlohmann::json::parse(CheckPlayerDeath(ply));
		}

		return res.dump();
	}

	std::string Game::Battle(std::shared_ptr<Player> ply, std::string enemyName) {
		/*
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

					if (rand() % 100 <= 50) {
						res += ply->battleStatus.enemy->Attack(ply);
						res += CheckPlayerDeath(ply);
					}

					break;
				}
			}
		}

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "Battle";
		res["content"]["enemyInBattle"] = false;
		res["content"]["enemyInBattlePlayerName"] = "";

		res["content"]["startedBattle"] = false;
		res["content"]["startedBattleWith"] = "";

		res["content"]["enemyStep"] = nlohmann::json::object();
		res["content"]["checkPlayerDeath"] = nlohmann::json::object();

		for (auto& it : ply->location->enemies) {
			if (it->name == enemyName) {
				if (it->battleWith != nullptr) {
					res["content"]["enemyInBattle"] = true;
					res["content"]["enemyInBattlePlayerName"] = it->battleWith->username;
				}
				else {
					ply->battleStatus.status = InBattle;
					ply->battleStatus.enemy = it;
					it->battleWith = ply;
					res["content"]["startedBattle"] = true;
					res["content"]["startedBattleWith"] = it->name;

					if (rand() % 100 <= 50) {
						res["content"]["enemyStep"] = nlohmann::json::parse(ply->battleStatus.enemy->Attack(ply));
						res["content"]["checkPlayerDeath"] = nlohmann::json::parse(CheckPlayerDeath(ply));
					}

					break;
				}
			}
		}

		return res.dump();
	}

	std::string Game::CheckPlayerDeath(std::shared_ptr<Player> ply) {
		/*
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
			ply->stats.weaponSkill /= 2;
			ply->stats.currentExp = 0;
			ply->stats.inventoryMaxSize /= 2; //configurator.config.maxInventorySize;

			for (auto& it : gamedata.floors) {
				if (it->level == 1) {
					ply->floor = it;
					ply->location = it->rooms[rand() % it->rooms.size()];
				}
			}
		}

		return res;*/


		nlohmann::json res;
		res["isDead"] = false;

		if (ply->location == nullptr) { return res.dump(); }

		res["x"] = ply->location->location.X;
		res["y"] = ply->location->location.X;
		res["Floor"] = ply->location->location.F;
		res["canRespawn"] = true;

		if (ply->stats.health <= 0) {
			res["isDead"] = true;
			ply->stats.isDead = true;
			//сделать проверку сдох от врага или самосбора!
			if (ply->battleStatus.status == InBattle) {
				ply->battleStatus.enemy->battleWith = nullptr;
				ply->battleStatus.enemy = nullptr;
				ply->battleStatus.status = NotInBattle;
			}

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
							phrase = std::vformat(std::string_view(it.itemDescs[rand() % (it.itemDescs.size()-1)]), std::make_format_args(ply->inventory[i]->name));
						}
					}
				}
				ply->location->itemDescription.push_back(RoomDescElement(ply->inventory[i]->ID, phrase));
				ply->inventory.erase(ply->inventory.begin() + i);
			}

			//stats reset

			ply->stats.health = 100;
			ply->stats.armor = 0;
			ply->stats.weaponSkill /= 2;
			ply->stats.currentExp = 0;

			if (samosborGoing) {
				res["canRespawn"] = false;
				return res.dump();
			}

			for (auto& it : gamedata.floors) {
				if (it->level == 1) {
					ply->floor = it;
					if (it->rooms.size() == 1) {
						ply->location = it->rooms[0];
					}
					else {
						ply->location = it->rooms[rand() % (it->rooms.size() - 1)];
					}
					ply->stats.isDead = false;
				}
			}
		}

		return res.dump(); 
	}

	std::string Game::Equip(std::shared_ptr<Player> ply, std::string wepName) {
		/*
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

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "Equip";
		res["content"]["equiped"] = false;
		res["content"]["canEquip"] = true;
		res["content"]["wep"] = "";

		for (auto& it : ply->inventory) {
			if (it->name == wepName) {
				Weapon* wep = dynamic_cast<Weapon*>(it.get());
				if (wep) {
					res["content"]["wep"] = wep->equip(ply);
					res["content"]["equiped"] = true;
					break;
				}
				else {
					res["content"]["canEquip"] = false;
				}
			}
		}

		return res.dump(); 
	}

	std::string Game::RepairExit(std::shared_ptr<Player> ply) {
		/*
		std::string res = "";

		ExitRoom* rmm = dynamic_cast<ExitRoom*>(ply->location.get());

		if (rmm != nullptr && rmm->isBroken == false) { return "Выход не сломан"; }
		if (rmm == nullptr) { return "В этой комнате нечего чинить"; }

		std::unordered_map<int, int> recipe;
		std::unordered_map<int, int> inv;

		//Repairing

		for (auto& it : rmm->repairRecipe) {
			recipe[it] += 1;
		}

		for (auto& it : ply->inventory) {
			inv[it->ID] += 1;
		}

		bool canRepair = true;

		for (auto [id, count] : recipe) {
			if (inv[id] < count) {
				canRepair = false;
				std::string f = "Недостаточно предметов для починки\nВам нужны:";
				size_t c = 1;
				for (auto [id, count] : recipe) {
					for (auto& it : configurator.config.items) {
						if (it->ID == id) {
							f += "\n" + std::to_string(c) + ". " + it->name + " (" + std::to_string(count) + " штуки)";
							++c;
						}
					}
				}
;				return f;
			}
		}

		if (canRepair) {
			for (auto [id, count] : recipe) {
				int delRemain = count;

				for (int i = ply->inventory.size() - 1; i >= 0 && delRemain > 0; i--) {
					if (ply->inventory[i]->ID == id) {
						res += "\nВы потеряли " + ply->inventory[i]->name;
						ply->inventory.erase(ply->inventory.begin() + i);
						--delRemain;
					}
				}
			}
		}

		rmm->isBroken = false;
		res += "\nВы починили выход";

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "RepairExit";
		res["content"]["isExitBroken"] = true;
		res["content"]["canFixThisRoom"] = true;
		res["content"]["haveResources"] = true;

		res["content"]["needRes"] = nlohmann::json::array();
		res["content"]["lostedItems"] = nlohmann::json::array();

		res["content"]["repaired"] = false;

		ExitRoom* rmm = dynamic_cast<ExitRoom*>(ply->location.get());

		if (rmm != nullptr && rmm->isBroken == false) { res["content"]["isExitBroken"] = false; return res.dump(); }
		if (rmm == nullptr) { res["content"]["canFixThisRoom"] = false; return res.dump(); }

		std::unordered_map<int, int> recipe;
		std::unordered_map<int, int> inv;

		//Repairing

		for (auto& it : rmm->repairRecipe) {
			recipe[it] += 1;
		}

		for (auto& it : ply->inventory) {
			inv[it->ID] += 1;
		}

		bool canRepair = true;

		for (auto [id, count] : recipe) {
			if (inv[id] < count) {
				canRepair = false;
				res["content"]["haveResources"] = false;
				for (auto [id, count] : recipe) {
					for (auto& it : configurator.config.items) {
						if (it->ID == id) {
							res["content"]["needRes"].push_back({
								{"name", it->name},
								{"count", count}
							});
						}
					}
				}
				return res.dump();
			}
		}

		if (canRepair) {
			for (auto [id, count] : recipe) {
				int delRemain = count;

				for (int i = ply->inventory.size() - 1; i >= 0 && delRemain > 0; i--) {
					if (ply->inventory[i]->ID == id) {
						res["content"]["lostedItems"].push_back(ply->inventory[i]->name);
						ply->inventory.erase(ply->inventory.begin() + i);
						--delRemain;
					}
				}
			}
		}

		rmm->isBroken = false;
		res["content"]["repaired"] = true;

		return res.dump(); 
	}

	std::string Game::CurrentPlayers(std::shared_ptr<Player> ply) {
		/*
		std::string res = "Игроки в текущей комнате:";
		size_t c = 1;
		for (auto& it : gamedata.players) {
			if (it->location->location == ply->location->location) {
				res += "\n" + std::to_string(c) + ". " + it->username;
			}
		}

		return res;*/

		nlohmann::json res;
		res["type"] = "ANSWER";
		res["content"]["type"] = "PlayersInRoom";
		res["content"]["players"] = nlohmann::json::array();

		for (auto& it : gamedata.players) {
			if (it->location->location == ply->location->location) {
				res["content"]["players"].push_back(it->username);
			}
		}

		return res.dump(); 
	}

	std::string Game::GetHelp(std::shared_ptr<Player> ply) {
		nlohmann::json res;

		res["type"] = "ANSWER";
		res["content"]["type"] = "Help";
		res["content"]["help"] = nlohmann::json::array();

		res["content"]["help"].push_back({
			{"name", "я"},
			{"description", "Показывает вашу текующую статистику"}
		});
		res["content"]["help"].push_back({
			{"name", "осмотреться"},
			{"description", "Показывает информацию о локации в которой вы находитесь, описание, предметы, враги и т.д."}
		});
		res["content"]["help"].push_back({
			{"name", "север/юг/запад/восток"},
			{"description", "4 команды которые используются для передвижения по карте, для передвижения нужно просто написать нужную сторону"}
		});
		res["content"]["help"].push_back({
			{"name", "создать [предмет]"},
			{"description", "Крафтит выбранный предмет из вещей которые есть у вас в инвентаре"}
		});
		res["content"]["help"].push_back({
			{"name", "выбросить [предмет]"},
			{"description", "Выбрасывает выбранный предмет из инвентаря в комнату"}
		});
		res["content"]["help"].push_back({
			{"name", "поднять [предмет]"},
			{"description", "Поднимает предмет на локации, вы не можете подбирать предметы если на локации есть враги"}
		});
		res["content"]["help"].push_back({
			{"name", "инвентарь"},
			{"description", "Ваши текущие предметы"}
		});
		res["content"]["help"].push_back({
			{"name", "вверх/вниз"},
			{"description", "2 команды которые используются для смены этажа, могут использоваться только в комнатах которые обозначены как выход"}
		});
		res["content"]["help"].push_back({
			{"name", "осмотреть [предмет/враг]"},
			{"description", "Выводит описание предмета из вашего инвентаря или врага на локации с его характеристиками"}
		});
		res["content"]["help"].push_back({
			{"name", "рецепты"},
			{"description", "Показывает доступные рецепты для создания"}
		});
		res["content"]["help"].push_back({
			{"name", "использовать [предмет]"},
			{"description", "Использует предмет"}
		});
		res["content"]["help"].push_back({
			{"name", "атаковать"},
			{"description", "Атакует врага, можно использовать только в битве и если у вас экипировано оружие"}
		});
		res["content"]["help"].push_back({
			{"name", "битва"},
			{"description", "Используется чтобы вступить в битву с врагом"}
		});
		res["content"]["help"].push_back({
			{"name", "экипировать"},
			{"description", "Используется чтобы экипировать оружие и впоследствии атаковать им во время битвы"}
		});
		res["content"]["help"].push_back({
			{"name", "починить"},
			{"description", "Используется для починки выхода если тот сломан"}
		});
		res["content"]["help"].push_back({
			{"name", "игроки"},
			{"description", "Возвращает текущих игроков в одной комнате с вами"}
		});

		return res.dump();
	}

	void Game::LoadConfig() {
		configurator.LoadConfig();
		//add commands

		CommandHandler& commandhandler = CommandHandler::Instance();

		//commandhandler.add("карта", [this](std::shared_ptr<Player> ply) {return this->Map(ply); }, 0, false);
		commandhandler.add("я", [this](std::shared_ptr<Player> ply) {return this->Me(ply); }, 0, true);
		commandhandler.add("осмотреться", [this](std::shared_ptr<Player> ply) {return this->Look(ply);}, 0, false);

		//commandhandler.add("идти", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Move(ply,arg); }, 1, false);

		commandhandler.add("север", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Move(ply, "север"); }, 0, false);
		commandhandler.add("юг", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Move(ply, "юг"); }, 0, false);
		commandhandler.add("запад", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Move(ply, "запад"); }, 0, false);
		commandhandler.add("восток", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Move(ply, "восток"); }, 0, false);

		commandhandler.add("создать", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Craft(ply, arg); }, 1, false);
		commandhandler.add("выбросить", [this](std::shared_ptr<Player> ply, std::string arg) {return this->DropItem(ply, arg); }, 1, false);

		commandhandler.add("поднять", [this](std::shared_ptr<Player> ply, std::string arg) {return this->PickupItem(ply, arg); }, 1, false);
		commandhandler.add("инвентарь", [this](std::shared_ptr<Player> ply) {return this->Inventory(ply); }, 0, true);
		commandhandler.add("вверх", [this](std::shared_ptr<Player> ply) {return this->ChangeFloor(ply, 1); }, 0, false);
		commandhandler.add("вниз", [this](std::shared_ptr<Player> ply) {return this->ChangeFloor(ply, 0); }, 0, false);
		commandhandler.add("игроки", [this](std::shared_ptr<Player> ply) {return this->CurrentPlayers(ply); }, 0, true);

		commandhandler.add("осмотреть", [this](std::shared_ptr<Player> ply, std::string arg) {return this->LookItem(ply, arg); }, 1, false);
		commandhandler.add("рецепты", [this](std::shared_ptr<Player> ply) {return this->EnableCrafts(ply); }, 0, false);
		commandhandler.add("использовать", [this](std::shared_ptr<Player> ply, std::string arg) {return this->UseItem(ply, arg); }, 1, true);
		commandhandler.add("починить", [this](std::shared_ptr<Player> ply, std::string arg) {return this->RepairExit(ply); }, 0, false);
		commandhandler.add("помощь", [this](std::shared_ptr<Player> ply) { return this->GetHelp(ply);},0,true);

		commandhandler.add("атаковать", [this](std::shared_ptr<Player> ply) {return this->Attack(ply); }, 0, true);
		commandhandler.add("битва", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Battle(ply, arg); }, 1, false);
		commandhandler.add("экипировать", [this](std::shared_ptr<Player> ply, std::string arg) {return this->Equip(ply, arg); }, 1, true);
		commandhandler.add("пропустить", [this](std::shared_ptr<Player> ply, std::string arg) {
			nlohmann::json res;
			res["type"] = "ANSWER";
			res["content"]["type"] = "Skip";
			res["content"]["inBattle"] = true;

			res["content"]["enemyStep"] = nlohmann::json::object();
			res["content"]["checkPlayerDeath"] = nlohmann::json::object();

			if (ply->battleStatus.status != InBattle) { res["content"]["inBattle"] = false; return res.dump(); }

			res["content"]["enemyStep"] = nlohmann::json::parse(ply->battleStatus.enemy->Attack(ply));
			res["content"]["checkPlayerDeath"] = nlohmann::json::parse(CheckPlayerDeath(ply));

			return res.dump();
			}, 0, true);
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

	void Game::SetPlayerLocation(Player* ply, Location loc) {
		for (auto& it : gamedata.floors) {
			if (it->level == loc.F) {
				for (auto& itt : it->rooms) {
					if (itt->location == loc) {
						ply->location = itt;
						ply->floor = it;
						std::cout << "Player set location on floor " << itt->location.F << "\n";
						break;
					}
				}
				break;
			}
		}
	}

	void Game::SetPlayerBattleEnemy(Player* ply, int enemyID) {
		for (auto& it : ply->location->enemies) {
			if (it->ID == enemyID) {
				if (it->battleWith == nullptr) {
					for (auto& itt : gamedata.players) {
						if (itt->username == ply->username) {
							it->battleWith = itt;
							ply->battleStatus.enemy = it;
						}
					}
				}
			}
		}
	}

	void Game::SaveGame(std::string filename) {
		//World save

		std::ofstream outFile(filename + ".GSAVE", std::ios::binary);
		uint32_t floorscount = static_cast<uint32_t>(gamedata.floors.size());
		outFile.write(reinterpret_cast<const char*>(&floorscount), sizeof(floorscount));

		for (auto& it : gamedata.floors) {
			it->save(outFile);
		}

		//EOF World save
		std::cout << "Save world in " << filename << ".GSAVE" << "\n";

		//Players save
		uint32_t playerscount = static_cast<uint32_t>(gamedata.players.size());
		outFile.write(reinterpret_cast<const char*>(&playerscount),sizeof(playerscount));

		for (auto& it : gamedata.players) {
			it->save(outFile);
		}

		//EOF Players save
		std::cout << "Save players in " << filename << ".GSAVE" << "\n";

		outFile.close();
	}

	void Game::LoadGame(std::string filename) {
		if (configurator.config.configLoaded == false) {
			std::cout << "Cant load game while config not loaded!\n";
			return;
		}

		/*if (isGenerated == true) {
			std::cout << "Cant load game while started!\n";
			return;
		}*/
		if (isReseted == false) {
			std::cout << "You need reset game before loading!\n";
			return;
		}
		//World load
		gamedata = GameData();

		try {
			std::ifstream infile(filename + ".GSAVE", std::ios::binary);

			if (!infile.is_open()) {
				std::cout << "File load error!\n";
				return;
			}

			uint32_t floorscount = static_cast<uint32_t>(gamedata.floors.size());
			infile.read(reinterpret_cast<char*>(&floorscount), sizeof(floorscount));

			for (int i = 0; i < floorscount; i++) {
				std::shared_ptr<Floor> flr = std::make_shared<Floor>(
					0,
					std::vector<std::shared_ptr<Room>>{},
					std::vector<std::vector<int>>{},
					Location{ 0, 0, 0 },
					true,
					true
				);
				flr->load(infile);
				gamedata.floors.push_back(std::move(flr));
			}

			//Player load

			uint32_t plyCount;
			infile.read(reinterpret_cast<char*>(&plyCount), sizeof(plyCount));

			for (int i = 0; i < plyCount; i++) {
				std::string baseName = "player";
				std::weak_ptr<Player> ply = SpawnPlayer(baseName);
				ply.lock()->load(infile);
			}

			std::cout << "Load " << filename << "\n";

			infile.close();

			isGenerated = true;
			srv->stopSamosbor(false);
		}
		catch (const std::exception& e) {
			std::cout << "Error!\n";
		}
	}

	std::string Game::RespawnPlayer(std::shared_ptr<Player> ply) {
		if (ply->stats.isDead != true) { return ""; }

		for (auto& it : gamedata.floors) {
			if (it->level == 1) {
				ply->floor = it;
				if (it->rooms.size() == 1) {
					ply->location = it->rooms[0];
				}
				else {
					ply->location = it->rooms[rand() % (it->rooms.size() - 1)];
				}
				ply->stats.isDead = false;
				nlohmann::json res;
				res["type"] = "ANSWER";
				res["content"]["type"] = "Respawn";
				return res.dump();
			}
		}
	}
}