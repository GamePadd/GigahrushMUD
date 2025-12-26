#include <iostream>
#include <string>
#include "asio.hpp"

class Client {
	private:
		std::string username;

		asio::io_context& io_context;
		asio::ip::tcp::socket socket;
		asio::ip::tcp::resolver resolver;

		char* ip;
		char* port;

		std::string recv_buffer;

	public:
		Client(asio::io_context& io_context_, char* ip, char* port);

		void Connect();
		std::string Send(std::string& data);
};