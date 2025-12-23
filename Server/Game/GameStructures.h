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
		std::vector<std::string> itemDescs; // Шаблон для описания положений предметов в комнате
		std::vector<std::string> enemiesDescs; // Шаблон для описания положений врагов в комнате
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

	class Item {
		public:
			int ID;
			std::string name;
			std::string description;
			std::string useDescription;
			Type type;
			bool canSpawn;

			Item(int, std::string, std::string, std::string, Type, bool);
			~Item();

			//std::string use(Player&);
	};

	class Enemy {
		public:
			int ID;
			std::string name;
			std::string description;
			//Location location;
			std::vector<std::string> replics; //Рандомные реплики определенного врага
			unsigned short int health;
			unsigned short int attack;
			std::vector<Item> loot;

			Enemy(int, std::string, std::string, std::vector<std::string>, unsigned short int, unsigned short int, std::vector<Item>);
			~Enemy();

			//std::string attack(Player&);
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