#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "Socket_client.hpp"

Socket_client::Socket_client()
{
}

Socket_client::Socket_client(int fd, const std::string & addr, 
								const std::string & port) :
	fd(fd),
	addr(addr),
	port(port),
	state(HEADERS)
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
	//request = ref.request;
	buffer_recv = ref.buffer_recv;
	buffer_send = ref.buffer_send;
	addr = ref.addr;
	port = ref.port;
	state = ref.state;
	return *this;
}

bool process_body(void)
{
}

/* parser */
bool Socket_client::build_request() 
{
	if (state == REQUEST_LINE)
		state = request.process_request_line(buffer_recv, &cursor);
	if (state == HEADERS)
		state = request.process_headers(buffer_recv, &cursor);
	if (state == BODY) {
		state = request.process_body(buffer_recv, &cursor);
}

/* builder */
void Socket_client::build_response(void)
{
	//return open("test.txt", O_RDONLY);
}

void Socket_client::what(void) const
{
	std::cout << "[" << fd << "] - " << addr << ":" << port;
}
