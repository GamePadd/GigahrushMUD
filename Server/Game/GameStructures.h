#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <fstream>

namespace Gigahrush {

	enum Status { InBattle, NotInBattle };
	//enum Type { Component, Weapon, Food, Healing };

	// Config Structures

	struct MapSize {
		short int FloorCount;
		short int X;
		short int Y;
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

	struct RoomUniqueItems {
		int ID;
		std::vector<int> itemsID;
	};

	struct SamosborEvent {
		int minInterval;
		int maxInterval;

		int minDuration;
		int maxDuration;

		std::vector<int> liveRoomsIDs;
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

		void save(std::ofstream& outfile) {
			uint32_t X32 = static_cast<uint32_t>(X);
			uint32_t Y32 = static_cast<uint32_t>(Y);
			uint32_t F32 = static_cast<uint32_t>(F);
			outfile.write(reinterpret_cast<const char*>(&X32), sizeof(X32));
			outfile.write(reinterpret_cast<const char*>(&Y32), sizeof(Y32));
			outfile.write(reinterpret_cast<const char*>(&F32), sizeof(F32));
		}

		void load(std::ifstream& infile) {
			uint32_t X32;
			uint32_t Y32;
			uint32_t F32;
			infile.read(reinterpret_cast<char*>(&X32), sizeof(X32));
			infile.read(reinterpret_cast<char*>(&Y32), sizeof(Y32));
			infile.read(reinterpret_cast<char*>(&F32), sizeof(F32));

			X = X32;
			Y = Y32;
			F = F32;
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
			std::string equip(std::shared_ptr<Player>&);
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
			short int health;
			short int attack;
			short int exp;
			std::vector<std::unique_ptr<Item>> loot;
			std::shared_ptr<Player> battleWith;

			Enemy(int, std::string, std::string, std::vector<std::string>, short int, short int, std::vector<std::unique_ptr<Item>>&&, short int);
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

		int expTolevelUp;
		int currentExp;

		int inventoryMaxSize;

		bool wepEq;
		int weaponEqID;

		//Skill stats
		int weaponSkill;

		void save(std::ofstream& outfile) {
			outfile.write(reinterpret_cast<const char*>(&health),sizeof(health));
			outfile.write(reinterpret_cast<const char*>(&armor), sizeof(armor));
			outfile.write(reinterpret_cast<const char*>(&level), sizeof(level));
			outfile.write(reinterpret_cast<const char*>(&expTolevelUp), sizeof(expTolevelUp));
			outfile.write(reinterpret_cast<const char*>(&currentExp), sizeof(currentExp));
			outfile.write(reinterpret_cast<const char*>(&inventoryMaxSize), sizeof(inventoryMaxSize));
			outfile.write(reinterpret_cast<const char*>(&wepEq), sizeof(wepEq));
			outfile.write(reinterpret_cast<const char*>(&weaponEqID), sizeof(weaponEqID));
			outfile.write(reinterpret_cast<const char*>(&weaponSkill), sizeof(weaponSkill));
		}

		void load(std::ifstream& infile) {
			infile.read(reinterpret_cast<char*>(&health), sizeof(health));
			infile.read(reinterpret_cast<char*>(&armor), sizeof(armor));
			infile.read(reinterpret_cast<char*>(&level), sizeof(level));
			infile.read(reinterpret_cast<char*>(&expTolevelUp), sizeof(expTolevelUp));
			infile.read(reinterpret_cast<char*>(&currentExp), sizeof(currentExp));
			infile.read(reinterpret_cast<char*>(&inventoryMaxSize), sizeof(inventoryMaxSize));
			infile.read(reinterpret_cast<char*>(&wepEq), sizeof(wepEq));
			infile.read(reinterpret_cast<char*>(&weaponEqID), sizeof(weaponEqID));
			infile.read(reinterpret_cast<char*>(&weaponSkill), sizeof(weaponSkill));
		}
	};

	struct Battle {
		Status status;
		std::shared_ptr<Enemy> enemy;

		void save(std::ofstream& outfile) {
			if (status == Status::InBattle) {
				outfile.put(1);
				uint32_t enID = static_cast<uint32_t>(enemy->ID);
				outfile.write(reinterpret_cast<const char*>(&enID), sizeof(enID));
			}
			else {
				outfile.put(0);
			}
		}
	};

	struct Player {
		std::string username;
		std::shared_ptr<Room> location;
		std::shared_ptr<Floor> floor;
		std::vector<std::unique_ptr<Item>> inventory;
		Battle battleStatus;
		PlayerStats stats;
		std::atomic<bool> isInSession;

		void save(std::ofstream& outfile);

		void load(std::ifstream& infile);
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

		virtual void save(std::ofstream& outfile) {
			uint32_t ID32 = static_cast<uint32_t>(ID);
			outfile.write(reinterpret_cast<const char*>(&ID32), sizeof(ID32));
			outfile.write(reinterpret_cast<const char*>(&isExit), sizeof(isExit));

			uint32_t itemCount = static_cast<uint32_t>(items.size());
			outfile.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
			for (auto& it : items) {
				uint32_t itID = static_cast<uint32_t>(it->ID);
				outfile.write(reinterpret_cast<const char*>(&itID), sizeof(itID));
			}

			uint32_t enemyCount = static_cast<uint32_t>(enemies.size());
			outfile.write(reinterpret_cast<const char*>(&enemyCount), sizeof(enemyCount));
			for (auto& it : enemies) {
				uint32_t itID = static_cast<uint32_t>(it->ID);
				outfile.write(reinterpret_cast<const char*>(&itID), sizeof(itID));
			}
		}

		virtual void load(std::ifstream& infile);

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

		virtual std::shared_ptr<Room> clone() {
			return std::make_shared<Room>(*this);
		}
	};

	//Типы комнат 1 - Обычная комната, просто room, тип 2 - выход (может ломаться)

	struct ExitRoom : public Room {
		bool isBroken;
		std::vector<int> repairRecipe;

		void save(std::ofstream& outfile) override {
			uint32_t ID32 = static_cast<uint32_t>(ID);
			outfile.write(reinterpret_cast<const char*>(&ID32), sizeof(ID32));
			outfile.write(reinterpret_cast<const char*>(&isExit), sizeof(isExit));
			outfile.write(reinterpret_cast<const char*>(&isBroken), sizeof(isBroken));

			uint32_t itemCount = static_cast<uint32_t>(items.size());
			outfile.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
			for (auto& it : items) {
				uint32_t itID = static_cast<uint32_t>(it->ID);
				outfile.write(reinterpret_cast<const char*>(&itID), sizeof(itID));
			}

			uint32_t enemyCount = static_cast<uint32_t>(enemies.size());
			outfile.write(reinterpret_cast<const char*>(&enemyCount), sizeof(enemyCount));
			for (auto& it : enemies) {
				uint32_t itID = static_cast<uint32_t>(it->ID);
				outfile.write(reinterpret_cast<const char*>(&itID), sizeof(itID));
			}
		}

		virtual void load(std::ifstream& infile) override;

		ExitRoom(int _ID, std::string _name, std::string _description,
			std::vector<RoomDescElement> _itemDescription,
			std::vector<RoomDescElement> _enemyDescription,
			std::vector<std::unique_ptr<Item>>&& _items,
			std::vector<std::shared_ptr<Enemy>> _enemies, bool _isExit, Location _location, bool _isBroken, std::vector<int> _repairRecipe) :
			Room(_ID, _name, _description, _itemDescription, _enemyDescription, std::move(_items), _enemies, _isExit, _location), 
			isBroken(_isBroken), repairRecipe(_repairRecipe) {}

		ExitRoom(const ExitRoom& other) : Room(other), isBroken(other.isBroken), repairRecipe(other.repairRecipe) {}

		ExitRoom& operator= (const ExitRoom& other) {
			if (this != &other) {
				Room::operator=(other); 
				isBroken = other.isBroken;
				repairRecipe = other.repairRecipe;
			}
			return *this;
		}

		std::shared_ptr<Room> clone() override {
			return std::make_shared<ExitRoom>(*this);
		}
	};

	struct Floor {
		int level;
		std::vector<std::shared_ptr<Room>> rooms;
		std::vector<std::vector<int>> floorMask;
		Location exitCoordinates;
		bool canGoUp = true;
		bool canGoDown = true;

		Floor(int _level, std::vector<std::shared_ptr<Room>> _rooms,
			std::vector<std::vector<int>> _floorMask, Location _exitCoordinates, bool _canGoUp, bool _canGoDown) :
			level(_level),
			rooms(_rooms),
			floorMask(_floorMask),
			exitCoordinates(_exitCoordinates),
			canGoUp(_canGoUp),
			canGoDown(_canGoDown) {}

		void save(std::ofstream& outfile) {
			uint32_t level32 = static_cast<uint32_t>(level);
			outfile.write(reinterpret_cast<const char*>(&level32), sizeof(level32));

			outfile.write(reinterpret_cast<const char*>(&canGoUp), sizeof(canGoUp));
			outfile.write(reinterpret_cast<const char*>(&canGoDown), sizeof(canGoDown));

			exitCoordinates.save(outfile);

			uint32_t sizeX = static_cast<uint32_t>(floorMask[0].size());
			uint32_t sizeY = static_cast<uint32_t>(floorMask.size());

			outfile.write(reinterpret_cast<const char*>(&sizeX), sizeof(sizeX));
			outfile.write(reinterpret_cast<const char*>(&sizeY), sizeof(sizeY));

			for (int y = 0; y < floorMask.size(); y++) {
				for (int x = 0; x < floorMask[0].size(); x++) {
					if (floorMask[y][x] == 0) {
						//No room
						outfile.put(0);
					}
					else {
						if (x == exitCoordinates.X && y == exitCoordinates.Y) {
							//exit
							outfile.put(2);
							for (auto& it : rooms) {
								if (it->location.X == x && it->location.Y == y) {
									it->save(outfile);
								}
							}
						}
						else {
							//just room
							outfile.put(1);
							for (auto& it : rooms) {
								if (it->location.X == x && it->location.Y == y) {
									it->save(outfile);
								}
							}
						}
					}
				}
			}
		}

		void load(std::ifstream&);
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