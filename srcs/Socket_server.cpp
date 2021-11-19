#include "Socket_server.hpp"

Socket_server::Socket_server(u_short port, uint32_t address) : port(port), address(address) {
	std::cout << "Socket_server()" << "\n";
}

void Socket_server::listen() {
	std::cout << "listen()" << "\n";
}

void Socket_server::bind() {
	std::cout << "bind()" << "\n";
}

void Socket_server::what() {
}

