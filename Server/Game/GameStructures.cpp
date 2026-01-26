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

	void Player::load(std::ifstream& infile) {
		uint32_t username_size;
		infile.read(reinterpret_cast<char*>(&username_size), sizeof(username_size));
		username.resize(username_size);
		infile.read(reinterpret_cast<char*>(username.data()), username_size);

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
	}
}