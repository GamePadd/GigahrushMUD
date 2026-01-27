#include "Session.h"
#include "Server.h"
#include "utils.h"

Server::Server(asio::io_context& io_context, std::uint16_t port) :
	io_context(io_context),
	acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
	timer1(io_context),
	samosborDuringTimer(io_context),
	samosborIntervalTimer(io_context),
	timer2(io_context) {}

void Server::async_accept() {
	socket.emplace(io_context);

	acceptor.async_accept(*socket, [&](asio::error_code error) {
		std::shared_ptr<Session> ses = std::make_shared<Session>(std::move(*socket), this);
		ses->start();
		allSessions.push_back(std::weak_ptr<Session>(ses));
		async_accept();
	});
}

void Server::mapUpdate(const asio::error_code& ec) {
	for (auto& it : allSessions) {
		try {
			if (auto p = it.lock()) {
				if (p->sessionPlayer.lock() != nullptr) {
					if (Gigahrush::Game::Instance().isGenerated == true) {
						if (p->socket.is_open()) {
							asio::write(p->socket, asio::buffer(Gigahrush::Game::Instance().Map(p->sessionPlayer.lock())));
						}
					}
				}
			}
		}
		catch (const std::exception& ec) {
			std::cout << ec.what() << std::endl;
		}
	}
}

void Server::startMapUpdate() {
	timer1.expires_after(asio::chrono::milliseconds(500));
	timer1.async_wait([this](const asio::error_code& ec) {
		if (!ec) { mapUpdate(ec); }
		else { return; }
		startMapUpdate();
	});
}

void Server::newPlayerNotify(std::string username) {
	nlohmann::json newply;
	newply["type"] = "SERVER";
	newply["content"]["type"] = "NewPlayer";
	newply["content"]["name"] = username;
	//std::cout << "notify start";
	for (auto& it : allSessions) {
		try {
			if (auto p = it.lock()) {
				if (p != nullptr) {
					if (p->socket.is_open()) {
						asio::write(p->socket, asio::buffer(newply.dump()));
					}
					//std::cout << "player notified";
				}
			}
		}
		catch (const std::exception& ec) {
			std::cout << ec.what() << std::endl;
		}
	}
}

void Server::disconnectPlayerNotify(std::string username) {
	nlohmann::json newply;
	newply["type"] = "SERVER";
	newply["content"]["type"] = "PlayerDisconnect";
	newply["content"]["name"] = username;
	//std::cout << "notify start";
	for (auto& it : allSessions) {
		try {
			if (auto p = it.lock()) {
				if (p != nullptr) {
					if (p->socket.is_open()) {
						asio::write(p->socket, asio::buffer(newply.dump()));
					}
					//std::cout << "player notified";
				}
			}
		}
		catch (const std::exception& ec) {
			std::cout << ec.what() << std::endl;
		}
	}
}

void Server::waitSamosbor() {

}

void Server::startSamosbor() {

}

void Server::stopSamosbor() {

}

void Server::checkPlayersSamosbor() {

}