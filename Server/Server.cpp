#include <iostream>

#include "Server.h"
#include "asio.hpp"
#include "nlohmann/json.hpp"
#include <optional>
#include <string>

class Session : public std::enable_shared_from_this<Session> {
	private:
		std::vector<char> buffer;
		asio::ip::tcp::socket socket;

	public:
		Session(asio::ip::tcp::socket&& socket) : socket(std::move(socket)) {
			std::cout << "User connected!" << std::endl; 
			buffer.resize(256);
		}

		void start() {
			read();
		}
		
		void read() {
			socket.async_read_some(asio::buffer(buffer),
				[self = shared_from_this()](std::error_code ec, std::size_t bytes_received) {
					if (!ec) {
						//std::cout << "\n=== DEBUG ===" << std::endl;
						//std::cout << "Bytes received: " << bytes_received << std::endl;
						std::cout.write(self->buffer.data(), bytes_received);
						asio::write(self->socket, asio::buffer(self->buffer.data(), bytes_received));
						self->read();
					}
				}
			);
		}
};

class Server {
	private:
		asio::io_context& io_context;
		asio::ip::tcp::acceptor acceptor;
		std::optional<asio::ip::tcp::socket> socket;

	public:
		Server(asio::io_context& io_context, std::uint16_t port) :
			io_context(io_context),
			acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}
		
		void async_accept() {
			socket.emplace(io_context);

			acceptor.async_accept(*socket, [&](asio::error_code error) {
				std::make_shared<Session>(std::move(*socket))->start();
				async_accept();
			});
		}
};

int main()
{
	asio::io_context io_context;
	Server srv(io_context, 15001);
	srv.async_accept();
	io_context.run();
	return 0;
}
