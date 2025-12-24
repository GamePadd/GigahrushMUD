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

		std::vector<std::unique_ptr<Item>> items;
		std::vector<std::unique_ptr<Enemy>> enemies;
		std::vector<std::unique_ptr<Room>> rooms;
	};

	class Configurator {
		public:
			Config config;

			void ShowAllConfig();
			void LoadConfig();
		private:
			void LoadMapSize();
			void LoadItems();
			void LoadEnemies();
			void LoadRooms();
			void LoadCrafts();
	};

	class Game {
		private:
			Game() {};
			~Game() {};

			Game(Game& const) = delete;
			Game& operator= (Game const&) = delete;

			void GenerateFloors();
			void GenerateRoom();
			void GenerateItemsAndEnemies();

		public:
			Configurator configurator;
			GameData gamedata;

			static Game& Instance();
			
			void GenerateGame();
	};
}