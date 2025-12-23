#include "GameStructures.h"

namespace Gigahrush {
	Item::Item(int _ID, std::string _name, std::string _description, std::string _useDescription, Type _type, bool _canSpawn) :
		ID(_ID),
		name(_name),
		description(_description),
		useDescription(_useDescription),
		type(_type),
		canSpawn(_canSpawn) {}
	Item::~Item() {}

	Enemy::Enemy(int _ID, std::string _name, std::string _description, std::vector<std::string> _replics, unsigned short int _health, unsigned short int _attack, std::vector<Item> _loot) :
		ID(_ID),
		name(_name),
		description(_description),
		replics(_replics),
		health(_health),
		attack(_attack),
		loot(_loot) {}
	Enemy::~Enemy() {}

}