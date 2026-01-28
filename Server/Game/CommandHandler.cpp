#include "CommandHandler.h"
#include <sstream>
#include <vector>
#include "nlohmann/json.hpp"

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

		nlohmann::json rs;
		rs["type"] = "ANSWER";
		rs["content"]["type"] = "";

		if (ply->stats.isDead == true) {
			rs["content"]["type"] = "youDead";
			return rs.dump();
		}

		if (comm == commands.end()) { rs["content"]["type"] = "unknown"; return rs.dump(); }
		if (commands[commandS].allowedInBattle == false && inBattle) { rs["content"]["type"] =  "inBattle"; return rs.dump();}
		if (commands[commandS].argc == 0) {
			return commands[commandS].func(ply, "");
		}
		else {
			if (arg == "") { rs["content"]["type"] = "badSyntax"; return rs.dump();}
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