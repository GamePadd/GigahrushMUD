#pragma once
#include <string>
#include "asio.hpp"
#include <iostream>
#include "Game/Game.h"
#include <memory>
#include <chrono>

class Session : public std::enable_shared_from_this<Session> {
	private:
		std::string buffer;
		Gigahrush::Game& game;

	public:
		asio::ip::tcp::socket socket;
		std::weak_ptr<Gigahrush::Player> sessionPlayer;

		Session(asio::ip::tcp::socket&& socket);
		~Session();

		void start();
		void read();
		void firstTime();
};