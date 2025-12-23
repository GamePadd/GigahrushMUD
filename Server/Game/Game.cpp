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

		std::cout << "=== items loaded ===" << std::endl;
		std::cout << "Items count: " << config.items.size() << std::endl;
	}

	void Configurator::LoadConfig() {
		std::cout << "=== Starting loading config ===" << std::endl;
		LoadMapSize();
		LoadItems();
		std::cout << "Config loaded!" << std::endl;
	}
}