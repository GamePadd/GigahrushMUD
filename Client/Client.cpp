#include <iostream>

#include "Client.h"
#include "asio.hpp"
#include "nlohmann/json.hpp"
#include <string>

int main()
{
	char recv_buffer[1024];
	std::uint16_t port = 15001;
	asio::io_context io_context;
	asio::ip::tcp::socket socket(io_context);
	asio::ip::tcp::resolver resolver(io_context);

	asio::connect(socket, resolver.resolve("localhost", "15001"));

	std::string data = "asdasd";
	asio::write(socket, asio::buffer(data));
	std::size_t len = socket.read_some(asio::buffer(recv_buffer));
	std::cout.write(recv_buffer, len);
	return 0;
}
