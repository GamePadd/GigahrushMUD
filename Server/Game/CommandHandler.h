#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <memory>
#include "GameStructures.h"

namespace Gigahrush {
	class Player;

	struct Command {
		std::function<std::string(std::shared_ptr<Player>, std::string)> func;
		int argc;
		bool allowedInBattle;
	};

	class CommandHandler {
		private:
			std::unordered_map<std::string, Command> commands;
			CommandHandler() = default;
		public:
			static CommandHandler& Instance() {
				static CommandHandler instance;
				return instance;
			}

			void add(const std::string&, std::function<std::string(std::shared_ptr<Player>)>, int, bool);
			void add(const std::string&, std::function<std::string(std::shared_ptr<Player>, std::string)>, int, bool);

			std::string handle(std::shared_ptr<Player>, std::string, bool);
	};
}