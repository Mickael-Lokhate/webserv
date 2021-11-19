#pragma once

#include <iostream>
#include <arpa/inet.h>

class Socket_server {

	public:
		int fd;
		u_short port;
		uint32_t address;

	public:
		Socket_server(u_short port, uint32_t address);
		void listen();
		void bind();
		void what();
};
