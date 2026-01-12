#include "Session.h"
#include "Server.h"

Server::Server(asio::io_context& io_context, std::uint16_t port) :
	io_context(io_context),
	acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
	timer1(io_context) {}

void Server::async_accept() {
	socket.emplace(io_context);

	acceptor.async_accept(*socket, [&](asio::error_code error) {
		std::shared_ptr<Session> ses = std::make_shared<Session>(std::move(*socket));
		ses->start();
		allSessions.push_back(std::move(ses));
		async_accept();
	});
}

void Server::timer1Exec(const asio::error_code& ec) {
	std::cout << "\nTest timer";
}

void Server::startTimer1() {
	timer1.expires_after(asio::chrono::seconds(2));
	timer1.async_wait([this](const asio::error_code& ec) {
		if (!ec) { timer1Exec(ec); }
		else { return; }
		startTimer1();
	});
}
