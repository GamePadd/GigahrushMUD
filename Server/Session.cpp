#include "Session.h"

Session::Session(asio::ip::tcp::socket&& socket, std::shared_ptr<Gigahrush::Player> ply) : 
	socket(std::move(socket)), 
	game(Gigahrush::Game::Instance()),
	sessionPlayer(ply) {
	buffer.resize(256);
}

Session::~Session() {
	std::cout << "User " << sessionPlayer->username << "disconnected!" << std::endl;
}

void Session::firstTime() {
	socket.async_read_some(asio::buffer(buffer),
		[self = shared_from_this()](std::error_code ec, std::size_t bytes_received) {
			if (!ec) {
				self->buffer.resize(bytes_received);
				if (self->buffer.data() != "") {
					std::size_t bt = asio::write(self->socket, asio::buffer("Вы успешно вошли!"));
					self->sessionPlayer->username = self->buffer;
					std::cout << "User has joined the game with nick: " << self->sessionPlayer->username << "\n";
					self->buffer.resize(256);
					self->read();
				}
				else {
					std::size_t bt = asio::write(self->socket, asio::buffer("Вы ввели пустой ник: "));
					self->buffer.resize(256);
					self->firstTime();
				}
			}
		}
	);
}

void Session::start() {
	firstTime();
}

void Session::read() {
	socket.async_read_some(asio::buffer(buffer),
		[self = shared_from_this()](std::error_code ec, std::size_t bytes_received) {
			if (!ec) {
				self->buffer.resize(bytes_received);
				std::string answer = self->game.ParseCommand(self->sessionPlayer);
				std::size_t bt = asio::write(self->socket, asio::buffer(answer));
				self->buffer.resize(256);
				self->read();
			}
		}
	);
}