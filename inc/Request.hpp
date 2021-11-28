#pragma once
#include <string>
#include <map>

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
	long								_content_length;
	bool								_chunked;
	std::map<std::string, std::string>	headers;
	std::string 						body;
	std::string							& buffer_recv;

	Request(std::string & buffer_recv);
//	Request(const Request &);
//	~Request(void);
//	Request & operator=(const Request & ref);
	void what(void) const;
	e_http_state process_headers(void);
	e_http_state process_body(void);
	e_http_state process_request_line(void);
};

