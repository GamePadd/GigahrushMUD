#include <string>
#include <vector>

namespace Gigahrush {
	enum Status { InBattle, NotInBattle };
	enum Type { Component, Weapon, Food, Healing };

	// Config Structures

	struct MapSize {
		unsigned short int FloorCount;
		unsigned short int X;
		unsigned short int Y;
	};

	struct SpawnChance {
		int ID;
		float chance;
	};

	struct RoomDesc {
		int ID;
		std::string desc;
		std::vector<std::string> itemDescs; // Шаблон для описания положений предметов в комнате
		std::vector<std::string> enemiesDescs; // Шаблон для описания положений врагов в комнате
	};

	struct ItemDesc {
		int ID;
		std::string desc;
	};

	struct EnemyDesc {
		int ID;
		std::string desc;
		std::vector<std::string> replics; //Рандомные реплики определенного врага
	};

	struct Craft {
		int ID;
		std::vector<int> craft;
	};

	// Game Structures

	struct Location {
		int X;
		int Y;
		int F;
	};

	class Enemy {
		public:
			int ID;
			std::string name;
			std::string description;
			Location location;
			unsigned short int health;
			unsigned short int attack;

			//std::string attack(Player&);
	};

	class Item {
		public:
			int ID;
			std::string name;
			std::string description;
			std::string useDescription;
			Type type;
			bool canSpawn;

			//std::string use(Player&);
	};

	struct Player {
		std::string username;
		Location location;
		std::vector<Item> inventory;
		Status status;
	};

	struct Room {
		int ID;
		std::string name;
		std::vector<std::string> description;
		std::vector<Item> items;
		std::vector<Enemy> enemies;
		bool isExit;
	};

	struct Floor {
		unsigned short int level;
		std::vector<std::vector<Room*>> rooms;
	};
}