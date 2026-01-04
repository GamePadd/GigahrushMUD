#include "CommandHandler.h"
#include <sstream>
#include <vector>

namespace Gigahrush {
	void CommandHandler::add(const std::string& name, std::function<std::string(std::shared_ptr<Player>)> func, int argc, bool allowedInBattle) {
		commands[name] = Command{ [func](std::shared_ptr<Player> ply, std::string arg) { return func(ply); }, argc, allowedInBattle };
	}

	void CommandHandler::add(const std::string& name, std::function<std::string(std::shared_ptr<Player>, std::string)> func, int argc, bool allowedInBattle) {
		commands[name] = Command{ [func](std::shared_ptr<Player> ply, std::string arg) { return func(ply, arg); }, argc, allowedInBattle };
	}

	std::string CommandHandler::handle(std::shared_ptr<Player> ply, std::string command, bool inBattle) {
		std::vector<std::string> args;
		std::stringstream ss(command);
		std::string commandS;
		std::string arg;
		ss >> commandS;

		std::getline(ss >> std::ws, arg);

		auto comm = commands.find(commandS);

		if (comm == commands.end()) { return "Неизвестная команда"; }
		if (commands[commandS].allowedInBattle == false && inBattle) { return "Вы не можете использовать эту команду в бою"; }
		if (commands[commandS].argc == 0) {
			return commands[commandS].func(ply, "");
		}
		else {
			if (arg == "") { return "Неправильный синтаксис"; }
			return commands[commandS].func(ply, arg);

			/*
			if (splitCommand.size() < 2) { return "Неправильный синтаксис"; }
			if (splitCommand[1] == "на") { 
				if (splitCommand.size() == 2) { return "Неправильный синтаксис"; }
				else { return commands[splitCommand[0]].func(ply, splitCommand[2]); }
			}
			return commands[splitCommand[0]].func(ply, splitCommand[1]);
			*/
		}
	}
}