#include <iostream>

#include "Server.h"
#include "Game/Game.h"

int main()
{
	Gigahrush::Game Game;
	Game.configurator.LoadConfig();

	asio::io_context io_context;
	Server srv(io_context, 15001);
	srv.async_accept();
	io_context.run();
	return 0;
}