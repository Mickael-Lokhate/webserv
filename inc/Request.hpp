#pragma once
#include <string>
#include <map>

class Request {

	public:

	std::string						method;
	std::string						uri;
	std::string						protocol;
	std::map<std::string, std::string>	headers;
	std::string 						body;

	Request(std::string & req_line,
			const std::map<std::string, std::string> & headers,
			const std::string & body);
	Request(const Request &);
	~Request(void);
	Request & operator=(const Request & ref);
	void what(void) const;
};

