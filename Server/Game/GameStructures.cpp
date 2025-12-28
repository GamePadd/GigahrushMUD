#include "GameStructures.h"

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
		return std::pair<std::string, bool>(std::string("Вы экипировали оружие!"), true);
	}

	std::pair<std::string, bool> Armor::use(std::shared_ptr<Player>& ply) const {
		return std::pair<std::string, bool>(std::string("Вы получили броню!"), true);
	}

	std::pair<std::string, bool> HealingItem::use(std::shared_ptr<Player>& ply) const {
		return std::pair<std::string, bool>(std::string("Вы вылечились!"), true);
	}

	//Descs

	std::string Component::getDescription() const {
		std::string res = "";
		res += "Предмет: " + name + "\nОписание: " + description + "\nНельзя использовать";
		return res;
	}

	std::string Weapon::getDescription() const {
		std::string res = "";
		res += "Предмет: " + name + "\nОписание: " + description + "\nУрон: " + std::to_string(damage) + "\nВы можете экипировать этот предмет";
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

	Enemy::Enemy(int _ID, std::string _name, std::string _description, std::vector<std::string> _replics, unsigned short int _health, unsigned short int _attack, std::vector<std::unique_ptr<Item>>&& _loot) :
		ID(_ID),
		name(_name),
		description(_description),
		replics(_replics),
		health(_health),
		attack(_attack),
		loot(std::move(_loot)) {}
	Enemy::~Enemy() {}

}