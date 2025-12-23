#include "GameStructures.h"

namespace Gigahrush {
	Item::Item(int _ID, std::string _name, std::string _description, std::string _useDescription, Type _type, bool _canSpawn) :
		ID(_ID),
		name(_name),
		description(_description),
		useDescription(_useDescription),
		type(_type),
		canSpawn(_canSpawn) {}
}