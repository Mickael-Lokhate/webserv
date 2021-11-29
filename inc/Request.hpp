#pragma once
#include <string>
#include <map>
#include "utils.hpp"

typedef enum http_state {
	REQUEST_LINE,
	HEADERS,
	BODY,
	RESPONSE,
	ERROR,
} e_http_state;

class Request {

	public:

	std::string							method;
	std::string							uri;
	std::string							host;
	long								content_length;
	bool								chunked;
	std::map<std::string, std::string>	headers;
	std::string 						body;
	std::string							delim;
	std::string							* buffer_recv;

	Request(std::string * buffer_recv);
//	Request(const Request &);
//	~Request(void);
	Request & operator=(const Request & ref);
	Request(Request & ref);
	void what(void) const;
	e_http_state process_headers(void);
	e_http_state process_body(void);
	e_http_state process_request_line(void);

	void mytolower(std::string & str);
	std::string  _statetostr(e_http_state st);
	bool get_ckunked_body(void);
	bool get_simple_body(void);
	ssize_t _hexstr_to_int(std::string const & hexstr);
	const std::string & check_method(void);
};
