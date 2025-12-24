#include <string>
#include <vector>
#include <memory>

namespace Gigahrush {
	enum Status { InBattle, NotInBattle };
	//enum Type { Component, Weapon, Food, Healing };

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

	struct Player;

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
			bool canSpawn;

			Item(int, std::string, std::string, std::string, bool);
			virtual ~Item();
			virtual std::unique_ptr<Item> clone() const = 0;
			virtual std::string use(Player&);
	};

	class Component : public Item {
		public:
			std::string use(Player&) override;

			Component(const Component& other) = default;
			std::unique_ptr<Item> clone() const override {
				return std::make_unique<Component>(*this);
			}

			Component(int, std::string, std::string, std::string, bool);
	};

	class Weapon : public Item {
		public:
			std::string use(Player&) override;

			Weapon(const Weapon& other) = default;
			std::unique_ptr<Item> clone() const override {
				return std::make_unique<Weapon>(*this);
			}

			Weapon(int, std::string, std::string, std::string, bool);
	};

	class Food : public Item {
		public:
			std::string use(Player&) override;

			Food(const Food& other) = default;
			std::unique_ptr<Item> clone() const override {
				return std::make_unique<Food>(*this);
			}

			Food(int, std::string, std::string, std::string, bool);
	};

	class HealingItem : public Item {
		public:
			std::string use(Player&) override;

			HealingItem(const HealingItem& other) = default;
			std::unique_ptr<Item> clone() const override {
				return std::make_unique<HealingItem>(*this);
			}

			HealingItem(int, std::string, std::string, std::string, bool);
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
			std::vector<std::unique_ptr<Item>> loot;

			Enemy(int, std::string, std::string, std::vector<std::string>, unsigned short int, unsigned short int, std::vector<std::unique_ptr<Item>>&&);
			~Enemy();

			//std::string attack(Player&);
	};

	struct Player {
		std::string username;
		Location location;
		std::vector<std::unique_ptr<Item>> inventory;
		Status status;
	};

	struct RoomDescElement {
		int ID;
		std::string desc;
	};

	struct Room {
		int ID;
		std::string name;
		std::string description;
		std::vector<RoomDescElement> itemDescription;
		std::vector<RoomDescElement> enemyDescription;
		std::vector<std::unique_ptr<Item>> items;
		std::vector<std::unique_ptr<Enemy>> enemies;
		bool isExit;
		Location location;

		Room(int _ID, std::string _name, std::string _description,
			std::vector<RoomDescElement> _itemDescription,
			std::vector<RoomDescElement> _enemyDescription,
			std::vector<std::unique_ptr<Item>>&& _items,
			std::vector<std::unique_ptr<Enemy>>&& _enemies, bool _isExit, Location _location) :
			ID(_ID),
			name(_name),
			description(_description),
			itemDescription(_itemDescription),
			enemyDescription(_enemyDescription),
			items(std::move(_items)),
			enemies(std::move(_enemies)),
			isExit(_isExit),
			location(_location) {}
	};

	struct Floor {
		unsigned short int level;
		std::vector<Room*> rooms;
		Location exitCoordinates;
	};

	//GameData

	struct GameData {
		std::vector<Floor> floors;
		std::vector<Player> players;
	};
}