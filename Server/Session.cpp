#include "Session.h"

Session::Session(asio::ip::tcp::socket&& socket) : socket(std::move(socket)) {
	std::cout << "User connected!" << std::endl;
	buffer.resize(256);
}

Session::~Session() {
	std::cout << "User disconnected!" << std::endl;
}

void Session::start() {
	read();
}

void Session::read() {
	socket.async_read_some(asio::buffer(buffer),
		[self = shared_from_this()](std::error_code ec, std::size_t bytes_received) {
			if (!ec) {
				std::cout << "Bytes received: " << bytes_received << std::endl;
				std::cout << "Data: ";
				std::cout.write(self->buffer.data(), bytes_received);
				std::cout << std::endl;

				asio::write(self->socket, asio::buffer(self->buffer.data(), bytes_received));
				self->read();
			}
		}
	);
}