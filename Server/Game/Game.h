#include "GameStructures.h"
#include "JsonParser.h"

namespace Gigahrush {
	struct Config {
		MapSize mapSize;

		std::vector<RoomDesc> roomDescs;

		std::vector<Craft> crafts;

		std::vector<SpawnChance> itemSpawnChances;
		std::vector<SpawnChance> roomSpawnChances;
		std::vector<SpawnChance> enemySpawnChances;

		std::vector<Item> items;
		std::vector<Enemy> enemies;
		std::vector<Room> rooms;
	};

	class Configurator {
		public:
			Config config;

			void LoadConfig();
		private:
			void LoadMapSize();
			void LoadItems();
	};

	class Game {
		public:
			Configurator configurator;
	};
}