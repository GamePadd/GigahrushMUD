#include <iostream>
#include <locale>

#include "Server.h"
#include "Game/Game.h"

int main()
{
	setlocale(LC_ALL, "ru_RU.utf8");
	Gigahrush::Game& Game = Gigahrush::Game::Instance();
	Game.configurator.LoadConfig();

	asio::io_context io_context;
	Server srv(io_context, 15001);
	srv.async_accept();
	io_context.run();
	return 0;
}