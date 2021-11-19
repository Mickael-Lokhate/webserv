#pragma once
#include <string>
struct Request {

	Request(const std::string &, const std::string &, const std::string &);
	Request(const Request &);
	~Request(void);
	Request & operator=(const Request &);

	what(void) const;

	std::string req_line;
	map<std::string, std::string> headers;
	std::string body;
};
