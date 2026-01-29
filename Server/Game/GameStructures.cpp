#include "GameStructures.h"
#include <algorithm>
#include "nlohmann/json.hpp"
#include "Game.h"

namespace Gigahrush {
	Item::Item(int _ID, const std::string& _name, const std::string& _description, const std::string& _useDescription, bool _canSpawn) :
		ID(_ID),
		name(_name),
		description(_description),
		useDescription(_useDescription),
		canSpawn(_canSpawn) {}
	Item::~Item() {}

	Component::Component(int _ID, std::string _name, std::string _description, std::string _useDescription, bool _canSpawn) :
		Item(_ID,_name,_description,_useDescription,_canSpawn) {}

	Weapon::Weapon(int _ID, std::string _name, std::string _description, std::string _useDescription, bool _canSpawn, int _damage) :
		Item(_ID, _name, _description, _useDescription, _canSpawn), damage(_damage) {}

	Armor::Armor(int _ID, std::string _name, std::string _description, std::string _useDescription, bool _canSpawn, int _armor) :
		Item(_ID, _name, _description, _useDescription, _canSpawn), armor(_armor) {}

	HealingItem::HealingItem(int _ID, std::string _name, std::string _description, std::string _useDescription, bool _canSpawn, int _heal) :
		Item(_ID, _name, _description, _useDescription, _canSpawn), heal(_heal) {}

	std::pair<std::string, bool> Component::use(std::shared_ptr<Player>& ply) const {
		nlohmann::json res;
		res["type"] = "Component";

		return std::pair<std::string, bool>(res.dump(), false);
	}

	std::pair<std::string, bool> Weapon::use(std::shared_ptr<Player>& ply) const {
		nlohmann::json res;
		res["type"] = "Weapon";

		return std::pair<std::string, bool>(res.dump(), false);
	}

	std::pair<std::string, bool> Armor::use(std::shared_ptr<Player>& ply) const {
		nlohmann::json res;
		res["type"] = "Armor";
		res["name"] = name;
		res["description"] = useDescription;
		res["armor"] = armor;
		res["used"] = true;

		if (ply->stats.armor >= 100) { 
			res["used"] = false;
			return std::pair<std::string, bool>(res.dump(), false);
		}

		ply->stats.armor = std::clamp(ply->stats.armor + armor, 0, 100);

		return std::pair<std::string, bool>(res.dump(), true);
	}

	std::pair<std::string, bool> HealingItem::use(std::shared_ptr<Player>& ply) const {
		nlohmann::json res;
		res["type"] = "Heal";
		res["name"] = name;
		res["description"] = useDescription;
		res["heal"] = heal;
		res["used"] = true;

		if (ply->stats.health >= 100) {
			res["used"] = false;
			return std::pair<std::string, bool>(res.dump(), false);
		}

		ply->stats.health = std::clamp(ply->stats.health + heal, 0, 100);

		return std::pair<std::string, bool>(res.dump(), true);
	}

	//Descs

	std::string Component::getDescription() const {
		/*
		std::string res = "";
		res += "Предмет: " + name + "\nОписание: " + description + "\nНельзя использовать";
		return res;*/

		nlohmann::json res;
		res["type"] = "Item";
		res["ItemType"] = "Component";
		res["name"] = name;
		res["description"] = description;
		
		return res.dump();
	}

	std::string Weapon::getDescription() const {
		/*
		std::string res = "";
		res += "Предмет: " + name + "\nОписание: " + description + "\nУрон: " + std::to_string(damage) + "\nВы можете атаковать этим предметом";
		return res;*/

		nlohmann::json res;
		res["type"] = "Item";
		res["ItemType"] = "Weapon";
		res["name"] = name;
		res["description"] = description;
		res["damage"] = damage;

		return res.dump();
	}

	std::string Armor::getDescription() const {
		/*std::string res = "";
		res += "Предмет: " + name + "\nОписание: " + description + "\nВосстановление брони: " + std::to_string(armor) + "\nМожно использовать";
		return res;*/

		nlohmann::json res;
		res["type"] = "Item";
		res["ItemType"] = "Armor";
		res["name"] = name;
		res["description"] = description;
		res["armor"] = armor;

		return res.dump();
	}

	std::string HealingItem::getDescription() const {
		/*std::string res = "";
		res += "Предмет: " + name + "\nОписание: " + description + "\nВосстановление здоровья: " + std::to_string(heal) + "\nМожно использовать";
		return res;*/

		nlohmann::json res;
		res["type"] = "Item";
		res["ItemType"] = "Heal";
		res["name"] = name;
		res["description"] = description;
		res["heal"] = heal;

		return res.dump();
	}

	Enemy::Enemy(int _ID, std::string _name, std::string _description, std::vector<std::string> _replics, short int _health, short int _attack, std::vector<std::unique_ptr<Item>>&& _loot, short int _exp) :
		ID(_ID),
		name(_name),
		description(_description),
		replics(_replics),
		health(_health),
		attack(_attack),
		loot(std::move(_loot)),
		exp(_exp) {}
	Enemy::~Enemy() {}

	std::string Enemy::Attack(std::shared_ptr<Player>& ply) {
		/*
		std::string res = "";

		if (replics.size() == 1) {
			res = "\nВраг сказал: " + replics[0];
		}
		else {
			res = "\nВраг сказал: " + replics[rand() % (replics.size() - 1)];
		}

		if (ply->stats.armor > 0) {
			ply->stats.health = std::clamp(ply->stats.health - (attack / 2), 0, 100);
			ply->stats.armor = std::clamp(ply->stats.armor - (attack / 2), 0, 100);

			res += "\n" + name + " ударил вас на " + std::to_string(attack / 2) + " урона и снёс вам " + std::to_string(attack / 2) + " единиц брони.\n"
				+ "Ваше здоровье: " + std::to_string(ply->stats.health) + "\nВаша броня: " + std::to_string(ply->stats.armor);
		}
		else {
			ply->stats.health = std::clamp(ply->stats.health - attack, 0, 100);
			res += "\n" + name + " ударил вас на " + std::to_string(attack) + " урона.\n"
				+ "Ваше здоровье: " + std::to_string(ply->stats.health) + "\nВаша броня: " + std::to_string(ply->stats.armor);
		}

		return res;*/

		nlohmann::json res;
		res["enemyName"] = name;
		res["hasArmor"] = false;
		res["replic"] = "";
		res["loseHealth"] = 0;
		res["loseArmor"] = 0;

		res["currentHealth"] = 0;
		res["currentArmor"] = 0;

		if (replics.size() == 1) {
			res["replic"] = replics[0];
		}
		else {
			res["replic"] = replics[rand() % (replics.size() - 1)];
		}

		if (ply->stats.armor > 0) {
			ply->stats.health = std::clamp(ply->stats.health - (attack / 2), 0, 100);
			ply->stats.armor = std::clamp(ply->stats.armor - (attack / 2), 0, 100);

			res["loseHealth"] = attack / 2;
			res["loseArmor"] = attack / 2;

			res["currentHealth"] = ply->stats.health;
			res["currentArmor"] = ply->stats.armor;

			res["hasArmor"] = true;
		}
		else {
			ply->stats.health = std::clamp(ply->stats.health - attack, 0, 100);

			res["loseHealth"] = attack;

			res["currentHealth"] = ply->stats.health;
			res["currentArmor"] = ply->stats.armor;
		}

		return res.dump();
	}

	std::string Weapon::equip(std::shared_ptr<Player>& ply) {
		std::string res = ""; 

		ply->stats.weaponEqID = ID;
		ply->stats.wepEq = true;

		res = name;
		return res;
	}

	void Player::save(std::ofstream& outfile) {
		uint32_t username_size = static_cast<uint32_t>(username.size());
		outfile.write(reinterpret_cast<const char*>(&username_size), sizeof(username_size));
		outfile.write(reinterpret_cast<const char*>(username.data()), username_size);

		location->location.save(outfile);

		stats.save(outfile);

		//Save inventory
		uint32_t inv_size = static_cast<uint32_t>(inventory.size());
		outfile.write(reinterpret_cast<const char*>(&inv_size), sizeof(inv_size));

		for (auto& it : inventory) {
			uint32_t itID = static_cast<uint32_t>(it->ID);
			outfile.write(reinterpret_cast<const char*>(&itID), sizeof(itID));
		}

		battleStatus.save(outfile);
	}

	void Player::load(std::ifstream& infile) {
		uint32_t username_size;
		infile.read(reinterpret_cast<char*>(&username_size), sizeof(username_size));
		username.resize(username_size);
		infile.read(reinterpret_cast<char*>(username.data()), username_size);
	
		Location loc;
		loc.load(infile);
		Game::Instance().SetPlayerLocation(this, loc);

		stats.load(infile);

		uint32_t inv_size;
		infile.read(reinterpret_cast<char*>(&inv_size), sizeof(inv_size));

		for (int i = 0; i < inv_size; i++) {
			uint32_t itemID;
			infile.read(reinterpret_cast<char*>(&itemID), sizeof(itemID));
			for (auto& it : Game::Instance().configurator.config.items) {
				if (itemID == it->ID) {
					inventory.push_back(it->clone());
				}
			}
		}

		uint8_t statuss;
		infile.read(reinterpret_cast<char*>(&statuss), sizeof(statuss));
		if (statuss == 1) {
			battleStatus.status = InBattle;

			uint32_t enID;
			infile.read(reinterpret_cast<char*>(&enID), sizeof(enID));

			Game::Instance().SetPlayerBattleEnemy(this, enID);
		}
		else {
			battleStatus.status = NotInBattle;
		}
	}

	void Room::load(std::ifstream& infile) {
		uint32_t itemCount;
		infile.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));

		for (int i = 0; i < itemCount; i++) {
			uint32_t itemID;
			infile.read(reinterpret_cast<char*>(&itemID), sizeof(itemID));
			for (auto& it : Game::Instance().configurator.config.items) {
				if (itemID == it->ID) {
					std::string phrase = "";

					for (auto& itt : Game::Instance().configurator.config.roomDescs) {
						if (itt.ID == ID) {
							if (itt.itemDescs.size() == 1) {
								phrase = std::vformat(std::string_view(itt.itemDescs[0]), std::make_format_args(it->name));
							}
							else {
								phrase = std::vformat(std::string_view(itt.itemDescs[rand() % (itt.itemDescs.size() - 1)]), std::make_format_args(it->name));
							}
							break;
						}
					}

					itemDescription.push_back(RoomDescElement(it->ID, phrase));
					items.push_back(it->clone());
				}
			}
		}

		uint32_t enemyCount;
		infile.read(reinterpret_cast<char*>(&enemyCount), sizeof(enemyCount));

		for (int i = 0; i < enemyCount; i++) {
			uint32_t enemyID;
			infile.read(reinterpret_cast<char*>(&enemyID), sizeof(enemyID));
			for (auto& it : Game::Instance().configurator.config.enemies) {
				if (enemyID == it->ID) {
					std::string phrase = "";

					for (auto& itt : Game::Instance().configurator.config.roomDescs) {
						if (itt.ID == ID) {
							if (itt.enemiesDescs.size() == 1) {
								phrase = std::vformat(std::string_view(itt.enemiesDescs[0]), std::make_format_args(it->name));
							}
							else {
								phrase = std::vformat(std::string_view(itt.enemiesDescs[rand() % (itt.enemiesDescs.size() - 1)]), std::make_format_args(it->name));
							}
							break;
						}
					}

					enemyDescription.push_back(RoomDescElement(it->ID, phrase));
					enemies.push_back(it->clone());
				}
			}
		}
	}

	void ExitRoom::load(std::ifstream& infile) {
		uint32_t itemCount;
		infile.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));

		for (int i = 0; i < itemCount; i++) {
			uint32_t itemID;
			infile.read(reinterpret_cast<char*>(&itemID), sizeof(itemID));
			for (auto& it : Game::Instance().configurator.config.items) {
				if (itemID == it->ID) {
					std::string phrase = "";

					for (auto& itt : Game::Instance().configurator.config.roomDescs) {
						if (itt.ID == ID) {
							if (itt.itemDescs.size() == 1) {
								phrase = std::vformat(std::string_view(itt.itemDescs[0]), std::make_format_args(it->name));
							}
							else {
								phrase = std::vformat(std::string_view(itt.itemDescs[rand() % (itt.itemDescs.size() - 1)]), std::make_format_args(it->name));
							}
							break;
						}
					}

					itemDescription.push_back(RoomDescElement(it->ID, phrase));
					items.push_back(it->clone());
				}
			}
		}

		uint32_t enemyCount;
		infile.read(reinterpret_cast<char*>(&enemyCount), sizeof(enemyCount));

		for (int i = 0; i < enemyCount; i++) {
			uint32_t enemyID;
			infile.read(reinterpret_cast<char*>(&enemyID), sizeof(enemyID));
			for (auto& it : Game::Instance().configurator.config.enemies) {
				if (enemyID == it->ID) {
					std::string phrase = "";

					for (auto& itt : Game::Instance().configurator.config.roomDescs) {
						if (itt.ID == ID) {
							if (itt.enemiesDescs.size() == 1) {
								phrase = std::vformat(std::string_view(itt.enemiesDescs[0]), std::make_format_args(it->name));
							}
							else {
								phrase = std::vformat(std::string_view(itt.enemiesDescs[rand() % (itt.enemiesDescs.size() - 1)]), std::make_format_args(it->name));
							}
							break;
						}
					}

					enemyDescription.push_back(RoomDescElement(it->ID, phrase));
					enemies.push_back(it->clone());
				}
			}
		}
	}

	void Floor::load(std::ifstream& infile) {
		uint32_t level32;
		infile.read(reinterpret_cast<char*>(&level32), sizeof(level32));
		infile.read(reinterpret_cast<char*>(&canGoUp), sizeof(canGoUp));
		infile.read(reinterpret_cast<char*>(&canGoDown), sizeof(canGoDown));
		level = level32;
		exitCoordinates.load(infile);

		//uint32_t sizeX = static_cast<uint32_t>(floorMask[0].size());
		//uint32_t sizeY = static_cast<uint32_t>(floorMask.size());

		uint32_t sizeX;
		uint32_t sizeY;

		infile.read(reinterpret_cast<char*>(&sizeX), sizeof(sizeX));
		infile.read(reinterpret_cast<char*>(&sizeY), sizeof(sizeY));

		floorMask.resize(sizeY, std::vector<int>(sizeX, 0));

		for (int y = 0; y < sizeY; y++) {
			for (int x = 0; x < sizeX; x++) {
				uint8_t roomType;
				infile.read(reinterpret_cast<char*>(&roomType), sizeof(roomType));

				if (roomType == 0) {
					floorMask[y][x] = 0;
				}
				else {
					if (roomType == 1) {
						floorMask[y][x] = 1;
						uint32_t roomID;
						infile.read(reinterpret_cast<char*>(&roomID), sizeof(roomID));
						for (auto& it : Game::Instance().configurator.config.rooms) {
							if (it->ID == roomID) {
								std::shared_ptr<Room> room = it->clone();
								bool isEx;
								infile.read(reinterpret_cast<char*>(&isEx), sizeof(isEx));
								room->isExit = isEx;
								room->location.X = x;
								room->location.Y = y;
								room->location.F = level;
								room->load(infile);
								rooms.push_back(std::move(room));
								break;
							}
						}
					}
					else if (roomType == 2) {
						floorMask[y][x] = 1;
						uint32_t roomID;
						infile.read(reinterpret_cast<char*>(&roomID), sizeof(roomID));
						for (auto& it : Game::Instance().configurator.config.rooms) {
							if (it->ID == roomID) {
								std::shared_ptr<Room> room = it->clone();
								ExitRoom* rm = dynamic_cast<ExitRoom*>(room.get());
								if (rm != nullptr) {
									bool isEx;
									infile.read(reinterpret_cast<char*>(&isEx), sizeof(isEx));
									bool isBroken;
									infile.read(reinterpret_cast<char*>(&isBroken), sizeof(isBroken));
									rm->isExit = isEx;
									rm->isBroken = isBroken;
									rm->location.X = x;
									rm->location.Y = y;
									rm->location.F = level;
									rm->load(infile);
									rooms.push_back(std::move(room));
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}