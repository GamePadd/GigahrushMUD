#include "Client.h"

Client::Client(asio::io_context& io_context_, char* ip, char* port) :
	io_context(io_context_),
	socket(io_context),
	resolver(io_context),
	ip(ip),
	port(port) {}

void Client::Connect() {
	asio::connect(socket, resolver.resolve(ip, port));
}

std::string Client::Send(std::string& data) {
	recv_buffer.resize(2048);
	std::size_t bytes_transmited = asio::write(socket, asio::buffer(data));
	std::size_t bytes_received = socket.read_some(asio::buffer(recv_buffer));
	recv_buffer.resize(bytes_received);
	return std::string(recv_buffer.data(), bytes_received);
}