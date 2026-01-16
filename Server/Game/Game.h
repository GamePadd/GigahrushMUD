#pragma once
#include "GameStructures.h"
#include "JsonParser.h"
#include <cstdlib>
#include <stdio.h>
#include <format>
#include <chrono>
#include <mutex>
#include <sstream>
#include <algorithm>
#include "CommandHandler.h"

namespace Gigahrush {

	struct Config {
		MapSize mapSize;

		std::vector<RoomDesc> roomDescs;

		std::vector<Craft> crafts;

		std::vector<SpawnChance> itemSpawnChances;
		std::vector<RoomUniqueItems> roomUniqueItems;

		std::vector<SpawnChance> roomSpawnChances;
		std::vector<SpawnChance> exitRoomSpawnChances;

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
			Game();
			~Game();

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
			void LoadConfig();

			void Info();

			std::string CheckPlayerDeath(std::shared_ptr<Player>);

			//Commands

			std::string CheckLevelUp(std::shared_ptr<Player>);

			std::string Map(std::shared_ptr<Player>); //JSON Done
			std::string Me(std::shared_ptr<Player>);  // JSON Done
			std::string Look(std::shared_ptr<Player>); //JSON Done
			std::string Move(std::shared_ptr<Player>, std::string); //JSON Done
			std::string Craft(std::shared_ptr<Player>, std::string); 
			std::string DropItem(std::shared_ptr<Player>, std::string); // JSON Done
			std::string PickupItem(std::shared_ptr<Player>, std::string); // JSON Done
			std::string Inventory(std::shared_ptr<Player>); // JSON Done
			std::string ChangeFloor(std::shared_ptr<Player>, int); 
			std::string LookItem(std::shared_ptr<Player>, std::string); 
			std::string EnableCrafts(std::shared_ptr<Player>); 
			std::string UseItem(std::shared_ptr<Player>, std::string); 
			std::string Attack(std::shared_ptr<Player>); // игрок, оружие 
			std::string Battle(std::shared_ptr<Player>, std::string); // игрок, враг 
			std::string Equip(std::shared_ptr<Player>, std::string); 
			std::string RepairExit(std::shared_ptr<Player>); 
			std::string CurrentPlayers(std::shared_ptr<Player>); 

			//EndCommands

			std::shared_ptr<Gigahrush::Player> SpawnPlayer(std::string&);
			std::string ParseCommand(std::shared_ptr<Gigahrush::Player>, std::string&);
	};
}