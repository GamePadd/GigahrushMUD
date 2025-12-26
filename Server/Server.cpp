#include "Session.h"
#include "Server.h"

Server::Server(asio::io_context& io_context, std::uint16_t port) :
	io_context(io_context),
	acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

void Server::async_accept() {
	socket.emplace(io_context);

	acceptor.async_accept(*socket, [&](asio::error_code error) {
		Gigahrush::Game& game = Gigahrush::Game::Instance();
		std::shared_ptr<Gigahrush::Player> ply = std::make_shared<Gigahrush::Player>();

		game.gamedata.players.push_back(ply);

		std::make_shared<Session>(std::move(*socket),ply)->start();
		async_accept();
	});
}
