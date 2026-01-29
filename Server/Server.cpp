#include "Session.h"
#include "Server.h"
#include "utils.h"

Server::Server(asio::io_context& io_context, std::uint16_t port) :
	io_context(io_context),
	acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
	timer1(io_context),
	samosborDuringTimer(io_context),
	samosborIntervalTimer(io_context),
	timer2(io_context),
	timer3(io_context) {}

void Server::async_accept() {
	socket.emplace(io_context);

	acceptor.async_accept(*socket, [&](asio::error_code error) {
		std::shared_ptr<Session> ses = std::make_shared<Session>(std::move(*socket), this);
		ses->start();
		allSessions.push_back(std::weak_ptr<Session>(ses));
		async_accept();
	});
}

void Server::sendChatMessage(std::shared_ptr<Gigahrush::Player> ply, std::string msg) {
	nlohmann::json res;
	res["type"] = "ANSWER";
	res["content"]["type"] = "Message";
	res["content"]["from"] = ply->username;
	res["content"]["msg"] = msg;

	for (auto& it : allSessions) {
		if (auto p = it.lock()) {
			if (p != nullptr) {
				if (p->socket.is_open()) {
					if (auto pl = p->sessionPlayer.lock()) {
						if (pl != nullptr) {
							if (pl->location == ply->location) {
								asio::write(p->socket, asio::buffer(res.dump()));
							}
						}
					}
				}
			}
		}
	}
}

void Server::notifyAll(const std::string& obj) {
	for (auto& it : allSessions) {
		try {
			if (auto p = it.lock()) {
				if (p != nullptr) {
					if (p->socket.is_open()) {
						asio::write(p->socket, asio::buffer(obj));
					}
				}
			}
		}
		catch (const std::exception& ec) {
			std::cout << ec.what() << std::endl;
		}
	}
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

	notifyAll(newply.dump());
}

void Server::disconnectPlayerNotify(std::string username) {
	nlohmann::json disply;
	disply["type"] = "SERVER";
	disply["content"]["type"] = "PlayerDisconnect";
	disply["content"]["name"] = username;

	notifyAll(disply.dump());
}

void Server::waitSamosbor() {
	if (Gigahrush::Game::Instance().isGenerated == false) {
		std::cout << "Cant start samosbor, game is not generated\n";
		return; 
	}

	std::cout << "Start wait samosbor\n";
	int waitTime = (Gigahrush::Game::Instance().configurator.config.samosborconfig.minInterval + rand() % (Gigahrush::Game::Instance().configurator.config.samosborconfig.maxInterval - Gigahrush::Game::Instance().configurator.config.samosborconfig.minInterval + 1))/2;
	
	nlohmann::json res;
	res["type"] = "SERVER";
	res["content"]["type"] = "SamosborWait";
	res["content"]["time"] = waitTime;

	std::vector<std::string> savePlaces;

	for (auto it : Gigahrush::Game::Instance().configurator.config.samosborconfig.liveRoomsIDs) {
		for (auto& itt : Gigahrush::Game::Instance().configurator.config.rooms) {
			if (itt->ID == it) {
				savePlaces.push_back(itt->name);
				break;
			}
		}
	}
	
	res["content"]["savePlaces"] = nlohmann::json::array();
	res["content"]["savePlaces"] = savePlaces;
	std::cout << "remain " << waitTime << " to start samosbor\n";
	notifyAll(res.dump());

	samosborIntervalTimer.expires_after(asio::chrono::seconds(waitTime));
	samosborIntervalTimer.async_wait([this](const asio::error_code& ec){
		startSamosbor();
	});
}

void Server::startSamosbor() {
	Gigahrush::Game::Instance().samosborGoing = true;
	std::cout << "Samosbor started\n";

	int waitTime = Gigahrush::Game::Instance().configurator.config.samosborconfig.minDuration + rand() % (Gigahrush::Game::Instance().configurator.config.samosborconfig.maxDuration - Gigahrush::Game::Instance().configurator.config.samosborconfig.minDuration + 1);

	nlohmann::json res;
	res["type"] = "SERVER";
	res["content"]["type"] = "SamosborStarted";

	notifyAll(res.dump());

	samosborDuringTimer.expires_after(asio::chrono::seconds(waitTime));
	samosborDuringTimer.async_wait([this](const asio::error_code& ec) {
		stopSamosbor(false);
	});
}

void Server::stopSamosbor(bool force) {
	Gigahrush::Game::Instance().samosborGoing = false;
	std::cout << "Samosbor stopped\n";

	nlohmann::json res;
	res["type"] = "SERVER";
	res["content"]["type"] = "SamosborEnded";

	notifyAll(res.dump());

	for (auto& it : allSessions) {
		try {
			if (auto p = it.lock()) {
				if (p->sessionPlayer.lock() != nullptr) {
					if (Gigahrush::Game::Instance().isGenerated == true) {
						if (p->socket.is_open()) {
							asio::write(p->socket, asio::buffer(Gigahrush::Game::Instance().RespawnPlayer(p->sessionPlayer.lock())));
						}
						else {
							std::string res = Gigahrush::Game::Instance().RespawnPlayer(p->sessionPlayer.lock());
						}
					}
				}
			}
		}
		catch (const std::exception& ec) {
			std::cout << ec.what() << std::endl;
		}
	}

	if (force) {return;}

	int waitTime = Gigahrush::Game::Instance().configurator.config.samosborconfig.minInterval + rand() % (Gigahrush::Game::Instance().configurator.config.samosborconfig.maxInterval - Gigahrush::Game::Instance().configurator.config.samosborconfig.minInterval + 1);
	timer3.expires_after(asio::chrono::seconds(waitTime));
	timer3.async_wait([this](const asio::error_code& ec) {
		waitSamosbor();
	});
}

void Server::checkPlayersSamosbor() {
	timer2.expires_after(asio::chrono::seconds(1));
	timer2.async_wait([this](const asio::error_code& ec) {
		if (ec) {
			return;
		}

		if (Gigahrush::Game::Instance().samosborGoing == false) { 
			checkPlayersSamosbor(); 
			return;
		}

		if (Gigahrush::Game::Instance().isGenerated == false) {
			checkPlayersSamosbor();
			return;
		}

		for (auto& it : allSessions) {
			try {
				if (auto p = it.lock()) {
					if (p->sessionPlayer.lock() != nullptr) {
						if (p->socket.is_open()) {
							//Test player samosbor death
							bool isDead = true;

							auto pp = p->sessionPlayer.lock();

							if (pp != nullptr) {
								for (auto it : Gigahrush::Game::Instance().configurator.config.samosborconfig.liveRoomsIDs) {
									if (pp->location == nullptr) {
										break;
									}

									if (pp->location->ID == it) {
										isDead = false;
										break;
									}
								}

								if (isDead == true) {
									if (pp->stats.isDead == true) {
										continue;
									}

									pp->stats.health = 0;
									pp->stats.armor = 0;
									pp->stats.isDead = true;

									nlohmann::json res;
									res["type"] = "ANSWER";
									res["content"]["type"] = "SamosborDeath";
									res["content"]["death"] = nlohmann::json::object();
									res["content"]["death"] = nlohmann::json::parse(Gigahrush::Game::Instance().CheckPlayerDeath(pp));

									asio::write(p->socket, asio::buffer(res.dump()));
								}
							}
						}
					}
				}
			}
			catch (const std::exception& ec) {
				std::cout << ec.what() << std::endl;
			}
		}
		checkPlayersSamosbor();
	});
}