#include "GameStructures.h"
#include "JsonParser.h"
#include <cstdlib>
#include <stdio.h>
#include <format>
#include <chrono>
#include <mutex>
#include <sstream>

namespace Gigahrush {
	struct Config {
		MapSize mapSize;

		std::vector<RoomDesc> roomDescs;

		std::vector<Craft> crafts;

		std::vector<SpawnChance> itemSpawnChances;
		std::vector<SpawnChance> roomSpawnChances;
		std::vector<SpawnChance> enemySpawnChances;

		std::vector<std::unique_ptr<Item>> items;
		std::vector<std::shared_ptr<Enemy>> enemies;
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
			std::shared_ptr<Room> GenerateRoom(Location, bool);
			void GenerateItemsAndEnemies(std::shared_ptr<Room>&);

			void PrintRoomInfo(std::shared_ptr<Room>&);
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

			//Commands

			std::string Map(std::shared_ptr<Player>);
			std::string Me(std::shared_ptr<Player>);
			std::string Look(std::shared_ptr<Player>);
			std::string Move(std::shared_ptr<Player>, std::string);
			std::string Craft(std::shared_ptr<Player>, std::string);
			std::string DropItem(std::shared_ptr<Player>, std::string);
			std::string PickupItem(std::shared_ptr<Player>, std::string);
			std::string Inventory(std::shared_ptr<Player>);

			//EndCommands

			std::shared_ptr<Gigahrush::Player> SpawnPlayer(std::string&);
			std::string ParseCommand(std::shared_ptr<Gigahrush::Player>, std::string&);
	};
}