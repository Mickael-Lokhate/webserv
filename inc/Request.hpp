#pragma once
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <algorithm>
#include "utils.hpp"

#define SPACE 1
#define COLON 1

class Request {

	public:

	std::string							method;
	std::string							uri;
	std::string							host;
	ssize_t								content_length;
	bool								chunked;
	std::map<std::string, std::string>	headers;
	std::string 						body;
	std::string							delim;
	std::string							* buffer_recv;
	short								error;

	Request();
	Request(std::string * buffer_recv);
	Request(const Request & ref);
	~Request(void);
	Request & operator=(const Request & ref);

	e_http_state process_request_line(void);
	e_http_state process_headers(void);
	e_http_state process_body(void);
	bool is_valid_uri(std::string const & str);
	bool get_ckunked_body(void);
	bool get_simple_body(void);

	const std::string & check_method(void);
	void what(void) const;
};
