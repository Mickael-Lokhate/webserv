#pragma once

#include <iostream>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "utils.hpp"

class Socket_server {

	public:
		int fd;
		std::string port;
		std::string address;
		static const int BACKLOG = 128;

	public:
		Socket_server(std::string const & port, std::string const & address);
		void listen_();
		void bind_();
		void what();
};
