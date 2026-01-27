#include "Session.h"
#include <nlohmann/json.hpp>

Session::Session(asio::ip::tcp::socket&& socket) : 
	socket(std::move(socket)), 
	game(Gigahrush::Game::Instance()) {
	sessionPlayer.reset();
	buffer.resize(256);
}

Session::~Session() {
	if (sessionPlayer.lock() != nullptr) {
		std::cout << "User " << sessionPlayer.lock()->username << " disconnected!" << std::endl;
		sessionPlayer.lock()->isInSession = false;
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

			nlohmann::json err;
			err["type"] = "ANSWER";
			err["content"]["type"] = "SessionError";
			err["content"]["text"] = "";

			self->buffer.resize(bytes_received);

			if (bytes_received == 0) {
				err["content"]["text"] = "Вы ввели пустой ник";
				std::size_t bt = asio::write(self->socket, asio::buffer(err.dump()));
				self->buffer.resize(256);
				self->firstTime();
				return;
			}

			std::string nickname = self->buffer;

			if (!Gigahrush::Game::Instance().isGenerated) {
				err["content"]["text"] = "Игра не сгенерирована, подождите";
				std::size_t bt = asio::write(self->socket, asio::buffer(err.dump()));
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
				err["content"]["text"] = "Данный игрок уже играет на сервере";
				std::size_t bt = asio::write(self->socket, asio::buffer(err.dump()));
				self->buffer.resize(256);
				self->firstTime();
				return;
			}

			if (isPlayerFound && !isFoundPlayerPlaying) {
				for (auto& it : Gigahrush::Game::Instance().gamedata.players) {
					if (it->username == nickname) {
						self->sessionPlayer = it;
						self->sessionPlayer.lock()->isInSession = true;
						std::cout << "User already in gamedata. User has joined the game with nick: " << self->sessionPlayer.lock()->username << "\n";
					}
				}
			}
			else {
				self->sessionPlayer = Gigahrush::Game::Instance().SpawnPlayer(self->buffer);
				self->sessionPlayer.lock()->isInSession = true;
				std::cout << "User has joined the game with nick: " << self->sessionPlayer.lock()->username << "\n";
			}

			self->buffer.resize(256);
			auto p = self->sessionPlayer.lock();
			if (p != nullptr) {
				std::string first = Gigahrush::Game::Instance().Look(self->sessionPlayer.lock());
				std::size_t bt = asio::write(self->socket, asio::buffer(first));
				self->read();
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
				nlohmann::json err;
				err["type"] = "ANSWER";
				err["content"]["type"] = "SessionError";
				err["content"]["text"] = "";

				if (self->sessionPlayer.expired()) { 
					//std::cout << "anal";
					self->sessionPlayer.reset();
					self->sessionPlayer.lock() = nullptr;
					err["content"]["text"] = "Ваш игрок был утерян в результате рестарта сервера, попробуйте снова\nВведите ник: ";
					std::size_t bt = asio::write(self->socket, asio::buffer(err.dump()));
					self->buffer.resize(256);
					self->firstTime(); 
				}

				self->buffer.resize(bytes_received);
				//std::cout << "\n\nREAD DEBUG\n\n";
				if (Gigahrush::Game::Instance().gamedata.players.size() != 0) {
					std::string answer = self->game.ParseCommand(self->sessionPlayer.lock(), self->buffer);
					std::size_t bt = asio::write(self->socket, asio::buffer(answer));
					self->buffer.resize(256);
					self->read();
				} 
				else {
					err["content"]["text"] = "Ваш игрок был утерян в результате рестарта сервера, попробуйте снова\nВведите ник: ";
					std::size_t bt = asio::write(self->socket, asio::buffer(err.dump()));
					self->buffer.resize(256);
					self->sessionPlayer.reset();
					self->sessionPlayer.lock() = nullptr;
					self->firstTime();
				}
			}
		}
	);
}