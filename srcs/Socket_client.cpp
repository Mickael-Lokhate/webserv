#include "Socket_client.hpp"

Socket_client::Socket_client(int fd) :
	fd(fd)
{
#ifdef DEBUG
	std::cout << "[Socket_client] - constructor default" << std::endl;
#endif
}

Socket_client::Socket_client(const Socket_client & ref)
{
#ifdef DEBUG
	std::cout << "[Socket_client] - constructor copy" << std::endl;
#endif
}

Socket_client::~Socket_client(void)
{
#ifdef DEBUG
	std::cout << "[Socket_client] - destructor" << std::endl;
#endif
}

Socket_client & Socket_client::operator=(const Socket_client & ref)
{
#ifdef DEBUG
	std::cout << "[Socket_client] - operator=" << std::endl;
#endif
	fd = ref.fd;
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
}
