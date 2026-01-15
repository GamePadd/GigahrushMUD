#include "Client.h"

Client::Client(asio::io_context& io_context_, std::string ip, std::string port) :
	io_context(io_context_),
	socket(io_context),
	resolver(io_context),
	ip(ip),
	port(port) {}

void Client::Connect() {
	asio::connect(socket, resolver.resolve(ip, port));
}

Client::~Client() {
	if (socket.is_open()) {
		socket.shutdown(asio::ip::tcp::socket::shutdown_both);
		socket.close();
	}
}

void Client::Send(std::string& data) {
	//recv_buffer.resize(2048);
	std::size_t bytes_transmited = asio::write(socket, asio::buffer(data));
	//std::size_t bytes_received = socket.read_some(asio::buffer(recv_buffer));
	//recv_buffer.resize(bytes_received);
	//return std::string(recv_buffer.data(), bytes_received);
}