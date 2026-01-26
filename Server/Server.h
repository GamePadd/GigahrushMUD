#pragma once
#include <optional>
#include "asio.hpp"
#include "Session.h"

class Server {
	private:
		asio::io_context& io_context;
		asio::ip::tcp::acceptor acceptor;
		std::optional<asio::ip::tcp::socket> socket;

		asio::steady_timer timer1;

	public:
		std::vector<std::weak_ptr<Session>> allSessions;
		Server(asio::io_context& io_context, std::uint16_t port);

		//Timers
		void mapUpdate(const asio::error_code&);

		void async_accept();
		void startMapUpdate();
};