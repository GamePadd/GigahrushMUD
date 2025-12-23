#include "Game.h"

namespace Gigahrush {
	void Configurator::LoadMapSize() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json SizeConfig = js.readFile("Config/MainConfig.json");
		config.mapSize.FloorCount = SizeConfig["FloorCount"];
		config.mapSize.X = SizeConfig["FloorSizeX"];
		config.mapSize.Y = SizeConfig["FloorSizeY"];

		std::cout << "=== MapSize config loaded ===" << std::endl;
		std::cout << "FloorCount: " << config.mapSize.FloorCount << std::endl;
		std::cout << "FloorSizeX: " << config.mapSize.X << std::endl;
		std::cout << "FloorSizeY: " << config.mapSize.Y << std::endl << std::endl;
	}

	void Configurator::LoadItems() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json ItemsConfig = js.readFile("Config/Items.json");

		for (int i = 0; i < ItemsConfig.size(); i++) {
			config.items.push_back(Item(
				ItemsConfig[i]["id"],
				ItemsConfig[i]["name"],
				ItemsConfig[i]["description"],
				ItemsConfig[i]["useDescription"],
				Type(ItemsConfig[i]["type"]),
				ItemsConfig[i]["canSpawn"]
			));
			config.itemSpawnChances.push_back(SpawnChance(ItemsConfig[i]["id"], ItemsConfig[i]["spawnChance"]));
		}

		std::cout << "=== Items loaded ===" << std::endl;
		std::cout << "Items count: " << config.items.size() << std::endl << std::endl;

	}

	void Configurator::LoadEnemies() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json EnemiesConfig = js.readFile("Config/Enemies.json");

		for (int i = 0; i < EnemiesConfig.size(); i++) {
			std::vector<Item> loot;

			for (int j = 0; j < EnemiesConfig[i]["loot"].size(); j++) {
				for (int v = 0; v < config.items.size(); v++) {
					if (config.items[v].ID == EnemiesConfig[i]["loot"][j]) {
						loot.push_back(config.items[v]);
					}
				}
			}

			config.enemies.push_back(Enemy(
				EnemiesConfig[i]["id"],
				EnemiesConfig[i]["name"],
				EnemiesConfig[i]["description"],
				EnemiesConfig[i]["replics"],
				EnemiesConfig[i]["health"],
				EnemiesConfig[i]["attack"], 
				loot
			));
			config.enemySpawnChances.push_back(SpawnChance(EnemiesConfig[i]["id"], EnemiesConfig[i]["spawnChance"]));
		}

		std::cout << "=== Enemies loaded ===" << std::endl;
		std::cout << "Enemies count: " << config.enemies.size() << std::endl << std::endl;
	}

	void Configurator::LoadRooms() {
		JSONParser& js = JSONParser::Instance();

		nlohmann::json RoomsConfig = js.readFile("Config/Rooms.json");

		for (int i = 0; i < RoomsConfig.size(); i++) {
			config.rooms.push_back(Room(
				RoomsConfig[i]["id"],
				RoomsConfig[i]["name"],
				std::vector<std::string>{RoomsConfig[i]["description"]},
				std::vector<Item>{},
				std::vector<Enemy>{},
				RoomsConfig[i]["isExit"]
			));

			config.roomDescs.push_back(RoomDesc(
				RoomsConfig[i]["id"],
				RoomsConfig[i]["itemDescs"],
				RoomsConfig[i]["enemiesDescs"]
			));
			config.roomSpawnChances.push_back(SpawnChance(RoomsConfig[i]["id"], RoomsConfig[i]["spawnChance"]));
		}
		std::cout << "=== Rooms loaded ===" << std::endl;
		std::cout << "Rooms count: " << config.rooms.size() << std::endl << std::endl;
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

	void Configurator::LoadConfig() {
		std::cout << "=== Starting loading config ===" << std::endl << std::endl;

		LoadMapSize();
		LoadItems();
		LoadEnemies();
		LoadRooms();
		LoadCrafts();

		std::cout << "Config loaded!" << std::endl;
	}
}