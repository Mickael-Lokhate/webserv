#include <iostream>
#include "Socket_client.hpp"

Socket_client::Socket_client(void)
{
}

Socket_client::Socket_client(int fd, const std::string & addr, 
								const std::string & port) :
	fd(fd),
	addr(addr),
	port(port)
{
}

Socket_client::Socket_client(const Socket_client & ref)
{
	*this = ref;
}

Socket_client::~Socket_client(void)
{
}

Socket_client & Socket_client::operator=(const Socket_client & ref)
{
	fd = ref.fd;
	addr = ref.addr;
	port = ref.port;
	return *this;
}

/* parser */
void Socket_client::build_request(const std::string & raw_data)
{
}

/* builder */
void Socket_client::build_response(void)
{
}

void Socket_client::what(void) const
{
	std::cout << "[" << fd << "] - " << addr << ":" << port << std::endl;
}
