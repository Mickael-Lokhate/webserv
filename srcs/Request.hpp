#pragma once
#include <string>
#include <map>

class Request {

	public:

	Request_line						req_line;
	std::map<std::string, std::string>	headers;
	std::string 						body;

	Request(Request_line & req_line,
			const std::map<std::string, std::string> & headers,
			const std::string & body);
	Request(const Request &);
	~Request(void);
	Request & operator=(const Request & ref);
	void what(void) const;
};
