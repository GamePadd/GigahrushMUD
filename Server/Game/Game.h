#include "GameStructures.h"
#include "JsonParser.h"
#include <cstdlib>

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

		std::size_t maxInventorySize;
		std::size_t maxRoomItems;

		bool configLoaded = false;
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
			std::unique_ptr<Room> GenerateRoom(Location);
			void GenerateItemsAndEnemies();

			bool changeDir(std::vector<std::vector<int>>&, int&, int&, int&);
			void GenerateBranchMask(std::vector<std::vector<int>>&, int, int, int, int);
			void GenerateFloorMask(std::vector<std::vector<int>>&, int, int, int, int);

		public:
			Configurator configurator;
			GameData gamedata;

			static Game& Instance();
			
			void GenerateGame();
	};
}