#pragma once
#include <iostream>
#include <string>
#include "asio.hpp"

class Client {
	private:
		std::string username;

		asio::io_context& io_context;
		asio::ip::tcp::resolver resolver;

	public:
		std::string ip;
		std::string port;

		std::string recv_buffer;
		std::string recv_buffer_server;

		asio::ip::tcp::socket socket;

		Client(asio::io_context& io_context_, std::string ip, std::string port);
		~Client();

		void Connect();
		void Send(std::string& data);
};