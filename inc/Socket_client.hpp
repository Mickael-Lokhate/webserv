#pragma once
#include <iostream>
#include <string>
#include "Request.hpp"

class Socket_client {	

	public:

	int				fd;
	std::string		buffer_recv;
	std::string		buffer_send;
	std::string		addr;
	std::string		port;
	e_http_state	state;
	long			cursor;
	Request			request;
	Response		response;
	Route			route;
    Socket_server   * sserver;

	Socket_client(void);
	Socket_client(int fd, const std::string & addr, 
						const std::string & port);
	Socket_client(const Socket_client & ref);
	~Socket_client(void);
	Socket_client & operator=(const Socket_client & ref);

	void build_request(void);
	void build_response(void);
	void what(void) const;
};
