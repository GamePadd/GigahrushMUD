#include "Game.h"

namespace Gigahrush {
	void Configurator::LoadMapSize() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json SizeConfig = js.readFile("Config/MainConfig.json");
		config.mapSize.FloorCount = SizeConfig["FloorCount"];
		config.mapSize.X = SizeConfig["FloorSizeX"];
		config.mapSize.Y = SizeConfig["FloorSizeY"];

		std::cout << "=== MapSize config loaded ===" << std::endl;
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
	}

	void Configurator::LoadConfig() {
		std::cout << "=== Starting loading config ===" << std::endl << std::endl;

		LoadMapSize();
		LoadItems();
		LoadEnemies();
		LoadRooms();
		LoadCrafts();

		std::cout << "Config loaded!\n\n";

		ShowAllConfig();
	}

	Game& Game::Instance() {
		static Game g;
		return g;
	}

	void Game::GenerateItemsAndEnemies() {

	}

	void Game::GenerateRoom() {

	}

	void Game::GenerateFloors() {

	}

	void Game::GenerateGame() {

	}
}