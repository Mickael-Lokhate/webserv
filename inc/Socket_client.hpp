#pragma once
#include <string>

class Socket_client {	

	public:

	std::string buffer_req;
	int			fd;

	Socket_client(int fd);
	Socket_client(const Socket_client & ref);
	~Socket_client(void);
	Socket_client & operator=(const Socket_client & ref);

	/* parser */
	void build_request(const std::string & raw_data);
	/* builder */
	void build_response(void);
	void what(void) const;


};


