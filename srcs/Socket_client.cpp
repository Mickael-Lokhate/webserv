#include "Socket_client.hpp"

Socket_client::Socket_client() : 
	fd(-1),
	buffer_recv(),
	addr("none"),
	port("nonw"),
	state(REQUEST_LINE),
	request(&buffer_recv)
{
	;
}

Socket_client::Socket_client(int fd, const std::string & addr, 
								const std::string & port) :
	fd(fd),
	buffer_recv(),
	addr(addr),
	port(port),
	state(REQUEST_LINE),
	request(&buffer_recv)
{
	;
}

Socket_client::Socket_client(const Socket_client & ref) :
	buffer_recv(),
	request(&buffer_recv) 
{
	*this = ref;
}

Socket_client::~Socket_client(void)
{
	;
}

Socket_client & Socket_client::operator=(const Socket_client & ref)
{
	fd = ref.fd;
	request = Request(&buffer_recv);
	buffer_recv = ref.buffer_recv;
	buffer_send = ref.buffer_send;
	addr = ref.addr;
	port = ref.port;
	state = ref.state;
	return *this;
}


void Socket_client::build_request() 
{
	if (state == REQUEST_LINE)
		state = request.process_request_line();
	if (state == HEADERS)
		state = request.process_headers();
	if (state == BODY) 
		state = request.process_body();
}

void Socket_client::build_response(void)
{
	;
}

void Socket_client::what(void) const
{
	std::cout << "[" << fd << "] - " << addr << ":" << port;
}
