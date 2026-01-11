#pragma once
#include <iostream>
#include <string>
#include "asio.hpp"

class Client {
	private:
		std::string username;

		asio::io_context& io_context;
		asio::ip::tcp::socket socket;
		asio::ip::tcp::resolver resolver;

		std::string recv_buffer;

	public:
		std::string ip;
		std::string port;

		Client(asio::io_context& io_context_, std::string ip, std::string port);
		void Connect();
		std::string Send(std::string& data);
};