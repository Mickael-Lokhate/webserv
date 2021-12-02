#pragma once
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <algorithm>
#include "utils.hpp"

class Response {

	public:

	short								status;
	std::map<std::string, std::string>	headers;
	std::string 						body;
	ssize_t								content_length;
	std::string							content_type;
	bool								chunked;
	std::string							location;

	Response();
	Response(const Response & ref);
	~Response(void);
	Response & operator=(const Response & ref);

	void what(void) const;
};
