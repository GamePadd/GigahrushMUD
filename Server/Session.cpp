#include "Session.h"

Session::Session(asio::ip::tcp::socket&& socket) : 
	socket(std::move(socket)), 
	game(Gigahrush::Game::Instance()),
	sessionPlayer(nullptr) {
	buffer.resize(256);
}

Session::~Session() {
	if (sessionPlayer != nullptr) {
		std::cout << "User " << sessionPlayer->username << " disconnected!" << std::endl;
		sessionPlayer->isInSession = false;
	}
	else {
		std::cout << "Unknown user disconnected!" << std::endl;
	}
}

void Session::firstTime() {
	socket.async_read_some(asio::buffer(buffer),
		[self = shared_from_this()](std::error_code ec, std::size_t bytes_received) {
			if (ec) {
				return;
			}

			self->buffer.resize(bytes_received);

			if (bytes_received == 0) {
				std::size_t bt = asio::write(self->socket, asio::buffer("Вы ввели пустой ник"));
				self->buffer.resize(256);
				self->firstTime();
				return;
			}

			std::string nickname = self->buffer;

			if (!Gigahrush::Game::Instance().isGenerated) {
				std::size_t bt = asio::write(self->socket, asio::buffer("Игра не сгенерирована, подождите"));
				self->buffer.resize(256);
				self->firstTime();
				return;
			}

			bool isPlayerFound = false;
			bool isFoundPlayerPlaying = false;

			for (auto& it : Gigahrush::Game::Instance().gamedata.players) {
				if (it->username == nickname) {
					isPlayerFound = true;
					isFoundPlayerPlaying = it->isInSession;
				}
			}

			if (isPlayerFound && isFoundPlayerPlaying) {
				std::size_t bt = asio::write(self->socket, asio::buffer("Данный игрок уже играет на сервере"));
				self->buffer.resize(256);
				self->firstTime();
				return;
			}

			if (isPlayerFound && !isFoundPlayerPlaying) {
				for (auto& it : Gigahrush::Game::Instance().gamedata.players) {
					if (it->username == nickname) {
						self->sessionPlayer = it;
						self->sessionPlayer->isInSession = true;
						std::cout << "User already in gamedata. User has joined the game with nick: " << self->sessionPlayer->username << "\n";
					}
				}
			}
			else {
				self->sessionPlayer = Gigahrush::Game::Instance().SpawnPlayer(self->buffer);
				self->sessionPlayer->isInSession = true;
				std::cout << "User has joined the game with nick: " << self->sessionPlayer->username << "\n";
			}

			self->buffer.resize(256);
			std::string first = Gigahrush::Game::Instance().Look(self->sessionPlayer);
			std::size_t bt = asio::write(self->socket, asio::buffer(first));

			self->read();
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
				//std::cout << "\n\nREAD DEBUG\n\n";
				if (Gigahrush::Game::Instance().gamedata.players.size() != 0) {
					std::string answer = self->game.ParseCommand(self->sessionPlayer, self->buffer);
					std::size_t bt = asio::write(self->socket, asio::buffer(answer));
					self->buffer.resize(256);
					self->read();
				} 
				else {
					std::size_t bt = asio::write(self->socket, asio::buffer("Ваш игрок был утерян в результате рестарта сервера, попробуйте снова\nВведите ник: "));
					self->buffer.resize(256);
					self->sessionPlayer = nullptr;
					self->firstTime();
				}
			}
		}
	);
}