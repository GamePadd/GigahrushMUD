#include <iostream>
#include <locale>
#include <thread>
#include <mutex>
#include <atomic>

#include "Server.h"
#include "Game/Game.h"

std::mutex gameMutex;
std::atomic<bool> serverRunning = false;

std::string toLowerCase(std::string str) {
	std::string res = "";
	for (auto c : str) {
		res += std::tolower(c);
	}
	return res;
}

void StartServer() {

	while (!serverRunning) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	asio::io_context io_context;
	Server srv(io_context, 15001);
	srv.async_accept();
	while (serverRunning) {
		io_context.poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	io_context.stop();
	io_context.poll();
	std::cout << "Server stopped\n";
}

void Terminal() {
	srand(time(0));
	Gigahrush::Game& Game = Gigahrush::Game::Instance();
	setlocale(LC_ALL, "ru_RU.utf8");

	std::thread serverThread(StartServer);

	while (true) {
		std::cout << "> ";
		std::string command;
		std::getline(std::cin, command);
		std::string lowCom = toLowerCase(command);

		if (lowCom == "loadconf") {
			gameMutex.lock();
			bool x = Game.configurator.LoadConfig();
			gameMutex.unlock();
		}
		else if (lowCom == "gen") {
			gameMutex.lock();
			bool x = Game.GenerateGame();
			gameMutex.unlock();
		}
		else if (lowCom == "start") {
			std::cout << "Starting server...\n";

			gameMutex.lock();
			if (Game.isGenerated) {
				serverRunning = true;
			}
			else {
				std::cout << "Can't start server. Game is not generated or loaded.\n";
			}
			gameMutex.unlock();
		}
		else if (lowCom == "stop") {
			std::cout << "Stopping server...\n";
			serverRunning = false;

			if (serverThread.joinable()) {
				serverThread.join();
			}
		}
		else if (lowCom == "reset") {
			std::cout << "Reseting game...\n";
		}
		else if (lowCom == "info") {
			gameMutex.lock();
			Game.Info();
			gameMutex.unlock();
		}
		else if (lowCom == "exit") {
			std::cout << "Stopping server...\n";
			serverRunning = false;

			if (serverThread.joinable()) {
				serverThread.join();
			}

			break;
		}
	}
}

int main()
{
	std::thread t1(Terminal);
	if (t1.joinable()) {
		t1.join();
	}
	return 0;
}