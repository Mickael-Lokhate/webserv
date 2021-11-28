#pragma once
#include <string>
#include <queue>
#include "Request.hpp"

typedef enum http_state { 
	HEADERS,
	BODY,
	RESPONSE,
	ERROR,
	SEND
} e_http_state;

class Socket_client {	

	public:

	int				fd;
	Request			request;
	std::string		buffer_recv;
	std::string		buffer_send;
	std::string		addr;
	std::string		port;
	e_http_state	state;

	Socket_client(void);
	Socket_client(int fd, const std::string & addr, 
					const std::string & port);
	Socket_client(const Socket_client & ref);
	~Socket_client(void);
	Socket_client & operator=(const Socket_client & ref);

	bool build_request(void);
	void build_response(void);
	void what(void) const;

	private: 

	bool process_headers(void);
	bool process_request_line(void);
};
