#pragma once
#include <string>

class Socket_client {	

	public:

	int			fd;
	std::string buffer_req;
	std::string addr;
	std::string port;

	Socket_client(int fd, const std::string & addr, 
					const std::string & port);
	Socket_client(const Socket_client & ref);
	~Socket_client(void);
	Socket_client & operator=(const Socket_client & ref);

	/* parser */
	void build_request(const std::string & raw_data);
	/* builder */
	void build_response(void);
	void what(void) const;


};


