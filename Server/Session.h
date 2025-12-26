#include <string>
#include "asio.hpp"
#include <iostream>
#include "Game/Game.h"
#include <memory>

class Session : public std::enable_shared_from_this<Session> {
private:
	std::string buffer;
	asio::ip::tcp::socket socket;
	Gigahrush::Game& game;
	std::shared_ptr<Gigahrush::Player> sessionPlayer;

	bool isAuth = false;

public:
	Session(asio::ip::tcp::socket&& socket, std::shared_ptr<Gigahrush::Player>);
	~Session();

	void start();
	void read();
	void firstTime();
};