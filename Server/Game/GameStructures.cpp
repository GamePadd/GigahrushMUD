#include "GameStructures.h"
#include <algorithm>

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
		return std::pair<std::string, bool>(std::string("Вы не можете использовать компонент!"), false);
	}

	std::pair<std::string, bool> Weapon::use(std::shared_ptr<Player>& ply) const {
		return std::pair<std::string, bool>(std::string("Вы не можете использовать оружие. Вы можете только атаковать им врага во время битвы!"), false);
	}

	std::pair<std::string, bool> Armor::use(std::shared_ptr<Player>& ply) const {
		ply->stats.armor += armor;
		std::string res = "Вы использовали " + name + ". \n" + useDescription + "\nВам добавлено " + std::to_string(armor) + " единиц брони.";
		return std::pair<std::string, bool>(res, true);
	}

	std::pair<std::string, bool> HealingItem::use(std::shared_ptr<Player>& ply) const {
		ply->stats.armor += heal;
		std::string res = "Вы использовали " + name + ". \n" + useDescription + "\nВам добавлено " + std::to_string(heal) + " единиц здоровья.";
		return std::pair<std::string, bool>(res, true);
	}

	//Descs

	std::string Component::getDescription() const {
		std::string res = "";
		res += "Предмет: " + name + "\nОписание: " + description + "\nНельзя использовать";
		return res;
	}

	std::string Weapon::getDescription() const {
		std::string res = "";
		res += "Предмет: " + name + "\nОписание: " + description + "\nУрон: " + std::to_string(damage) + "\nВы можете атаковать этим предметом";
		return res;
	}

	std::string Armor::getDescription() const {
		std::string res = "";
		res += "Предмет: " + name + "\nОписание: " + description + "\nВосстановление брони: " + std::to_string(armor) + "\nМожно использовать";
		return res;
	}

	std::string HealingItem::getDescription() const {
		std::string res = "";
		res += "Предмет: " + name + "\nОписание: " + description + "\nВосстановление здоровья: " + std::to_string(heal) + "\nМожно использовать";
		return res;
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
		std::string res = "";

		if (replics.size() == 1) {
			res = "\nВраг сказал: " + replics[0] + "\n";
		}
		else {
			res = "\nВраг сказал: " + replics[rand() % (replics.size() - 1)] + "\n";
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

		return res;
	}

	std::string Weapon::equip(std::shared_ptr<Player>& ply) {
		std::string res = ""; 

		ply->stats.weaponEqID = ID;
		ply->stats.wepEq = true;

		res = "Вы экипировали " + name;
		return res;
	}
}