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
	struct Room;
	struct Floor;

	struct Location {
		int X;
		int Y;
		int F;

		bool operator== (const Location& xd) const {
			return (X == xd.X && Y == xd.Y && F == xd.F);
		}
	};

	class Item {
		public:
			int ID;
			std::string name;
			std::string description;
			std::string useDescription;
			bool canSpawn;

			Item(int, const std::string&, const std::string&, const std::string&, bool);

			Item(const Item& other) : ID(other.ID),
				name(other.name),
				description(other.description),
				useDescription(other.useDescription),
				canSpawn(other.canSpawn) {}


			virtual ~Item();
			virtual std::unique_ptr<Item> clone() const = 0;
			virtual std::string getDescription() const = 0;
			virtual std::pair<std::string, bool> use(std::shared_ptr<Player>&) const = 0;
	};

	class Component : public Item {
		public:
			std::pair<std::string, bool> use(std::shared_ptr<Player>&) const override;
			std::string getDescription() const override;

			Component(const Component& other) : Item(other) {};
			std::unique_ptr<Item> clone() const override {
				return std::make_unique<Component>(*this);
			}

			Component(int, std::string, std::string, std::string, bool);
	};

	class Weapon : public Item {
		public:
			int damage;

			std::pair<std::string, bool> use(std::shared_ptr<Player>&) const override;
			std::string getDescription() const override;

			Weapon(const Weapon& other) : Item(other), damage(other.damage) {};
			std::unique_ptr<Item> clone() const override {
				return std::make_unique<Weapon>(*this);
			}

			Weapon(int, std::string, std::string, std::string, bool, int);
	};

	class Armor : public Item {
		public:
			int armor;

			std::pair<std::string, bool> use(std::shared_ptr<Player>&) const override;
			std::string getDescription() const override;

			Armor(const Armor& other) : Item(other), armor(other.armor) {};
			std::unique_ptr<Item> clone() const override {
				return std::make_unique<Armor>(*this);
			}

			Armor(int, std::string, std::string, std::string, bool, int);
	};

	class HealingItem : public Item {
		public:
			int heal;

			std::pair<std::string, bool> use(std::shared_ptr<Player>&) const override;
			std::string getDescription() const override;

			HealingItem(const HealingItem& other) : Item(other), heal(other.heal) {};
			std::unique_ptr<Item> clone() const override {
				return std::make_unique<HealingItem>(*this);
			}

			HealingItem(int, std::string, std::string, std::string, bool, int);
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
			unsigned short int exp;
			std::vector<std::unique_ptr<Item>> loot;
			std::shared_ptr<Player> battleWith;

			Enemy(int, std::string, std::string, std::vector<std::string>, unsigned short int, unsigned short int, std::vector<std::unique_ptr<Item>>&&, unsigned short int);
			~Enemy();

			Enemy(const Enemy& other) :
				ID(other.ID),
				name(other.name),
				description(other.description),
				replics(other.replics),
				health(other.health),
				attack(other.attack),
				exp(other.exp) {
				for (auto &it : other.loot) {
					if (it) {
						loot.push_back(it->clone());
					}
				}
			}

			Enemy& operator= (const Enemy& other) {
				if (this != &other) {
					ID = other.ID;
					name = other.name;
					description = other.description;
					replics = other.replics;
					health = other.health;
					attack = other.attack;
					exp = other.exp;

					loot.clear();

					for (auto& it : other.loot) {
						if (it) {
							loot.push_back(it->clone());
						}
					}
				}
				return *this;
			}

			std::unique_ptr<Enemy> clone() {
				return std::make_unique<Enemy>(*this);
			}

			std::string Attack(std::shared_ptr<Player>&);
	};

	struct PlayerStats {
		//Main stats
		int health;
		int armor;

		int level;
		int inventoryMaxSize;

		bool wepEq;
		int weaponEqID;

		//Skill stats
		int weaponSkill;
	};

	struct Battle {
		Status status;
		std::shared_ptr<Enemy> enemy;
	};

	struct Player {
		std::string username;
		std::shared_ptr<Room> location;
		std::shared_ptr<Floor> floor;
		std::vector<std::unique_ptr<Item>> inventory;
		Battle battleStatus;
		PlayerStats stats;
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
		std::vector<std::shared_ptr<Enemy>> enemies;
		bool isExit;
		Location location;

		Room(int _ID, std::string _name, std::string _description,
			std::vector<RoomDescElement> _itemDescription,
			std::vector<RoomDescElement> _enemyDescription,
			std::vector<std::unique_ptr<Item>>&& _items,
			std::vector<std::shared_ptr<Enemy>> _enemies, bool _isExit, Location _location) :
			ID(_ID),
			name(_name),
			description(_description),
			itemDescription(_itemDescription),
			enemyDescription(_enemyDescription),
			items(std::move(_items)),
			enemies(_enemies),
			isExit(_isExit),
			location(_location) {}

		Room(const Room& other) :
			ID(other.ID),
			name(other.name),
			description(other.description),
			itemDescription(other.itemDescription),
			enemyDescription(other.enemyDescription),
			isExit(other.isExit),
			location(other.location) {
			for (auto &it : other.items) {
				if (it) {
					items.push_back(it->clone());
				}
			}

			for (auto& it : other.enemies) {
				if (it) {
					enemies.push_back(it->clone());
				}
			}
		}

		Room& operator= (const Room& other) {
			if (this != &other) {
				ID = other.ID;
				name = other.name;
				description = other.description;
				itemDescription = other.itemDescription;
				enemyDescription = other.enemyDescription;
				isExit = other.isExit;
				location = other.location;

				items.clear();

				for (auto& it : other.items) {
					if (it) {
						items.push_back(it->clone());
					}
				}

				enemies.clear();

				for (auto& it : other.enemies) {
					if (it) {
						enemies.push_back(it->clone());
					}
				}
			}

			return *this;
		}

		std::shared_ptr<Room> clone() {
			return std::make_shared<Room>(*this);
		}
	};

	struct Floor {
		unsigned short int level;
		std::vector<std::shared_ptr<Room>> rooms;
		std::vector<std::vector<int>> floorMask;
		Location exitCoordinates;
		bool canGoUp = true;
		bool canGoDown = true;

		Floor(unsigned short int _level, std::vector<std::shared_ptr<Room>> _rooms,
			std::vector<std::vector<int>> _floorMask, Location _exitCoordinates, bool _canGoUp, bool _canGoDown) :
			level(_level),
			rooms(_rooms),
			floorMask(_floorMask),
			exitCoordinates(_exitCoordinates),
			canGoUp(_canGoUp),
			canGoDown(_canGoDown) {}
		/*
		Floor(const Floor& other) :
			level(other.level),
			rooms(std::move(rooms)),
			floorMask(other.floorMask),
			exitCoordinates(other.exitCoordinates),
			canGoUp(other.canGoUp),
			canGoDown(other.canGoDown) {}

		Floor& operator= (const Floor& other) {

		}*/
	};

	//GameData

	struct GameData {
		std::vector<std::shared_ptr<Floor>> floors;
		std::vector<std::shared_ptr<Player>> players;
	};
}