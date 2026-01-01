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
		std::vector<std::string> splitCommand;
		std::stringstream ss(command);
		std::string word;

		while (ss >> word) {
			splitCommand.push_back(word);
		}

		auto comm = commands.find(splitCommand[0]);

		if (comm == commands.end()) { return "Неизвестная команда"; }
		if (commands[splitCommand[0]].allowedInBattle == false && inBattle) { return "Вы не можете использовать эту команду в бою"; }
		if (commands[splitCommand[0]].argc == 0) {
			return commands[splitCommand[0]].func(ply, "");
		}
		else {
			if (splitCommand.size() < 2) { return "Неправильный синтаксис"; }
			if (splitCommand[1] == "на") { 
				if (splitCommand.size() == 2) { return "Неправильный синтаксис"; }
				else { return commands[splitCommand[0]].func(ply, splitCommand[2]); }
			}
			return commands[splitCommand[0]].func(ply, splitCommand[1]);
		}
	}
}