#include "GameStructures.h"
#include "JsonParser.h"
#include <cstdlib>
#include <stdio.h>
#include <format>
#include <chrono>
#include <mutex>

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
			bool LoadConfig();
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
			std::unique_ptr<Room> GenerateRoom(Location, bool);
			void GenerateItemsAndEnemies(std::unique_ptr<Room>&);

			void PrintRoomInfo(std::unique_ptr<Room>&);
			bool changeDir(std::vector<std::vector<int>>&, int&, int&, int&);
			void GenerateBranchMask(std::vector<std::vector<int>>&, int, int, int, int);
			void GenerateFloorMask(std::vector<std::vector<int>>&, int, int, int, int);

			std::mutex game_mutex;

		public:
			Configurator configurator;
			GameData gamedata;
			bool isGenerated;

			static Game& Instance();
			
			bool GenerateGame();
			void ResetGame();

			void Info();

			std::string ParseCommand();
	};
}