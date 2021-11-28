#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "Request.hpp"
#define SPACE 1
#define TRAIL 3
#define COLON 1
#define DELIM 2

Request::Request(std::string & buffer_recv) : buffer_recv(buffer_recv) {
}

static const std::string & check_method
(std::string & buffer_recv)
{
	static const std::string	methods[] = {
		"GET", "POST", "PUT", "DELETE", "err"};
	int i;

	for (i = 0; i < 4; i++)
		if (!buffer_recv.compare(0, methods[i].size(),
								methods[i]))
			break;
	return (methods[i]);
}

e_http_state Request::process_request_line(std::string & buffer_recv, int *cursor)
{
	static const std::string	version = "HTTP/1.";
	size_t 						found;

	found = buffer_recv.find("\r\n");
	if (found == std::string::npos)
		return REQUEST_LINE;
	method = check_method(buffer_recv);
	if (method == "err" || buffer_recv[method.size()] != ' ')
		return ERROR;
	found = buffer_recv.find(" ", method.size() + SPACE);
	if (found == std::string::npos)
		return ERROR;
	uri = buffer_recv.substr(method.size() + SPACE,
		   					found - (method.size() + SPACE));
	found = buffer_recv.find(version, method.size() + uri.size() +
							SPACE * 2);
	if ((found == std::string::npos) ||
		/* doest minor version is a digit ? */
		!isdigit(buffer_recv[found + version.size()]) ||
		/* [METHOD SPACE URI SPACE HTTP/x.x\r\n */
		((method.size() + uri.size() + version.size() +
		2 * SPACE + TRAIL) != buffer_recv.size()))
		return ERROR;
#ifdef DEBUG
	std::cout << "URI is [" << uri << "]\n";
#endif
	return HEADERS;
}

e_http_state Request::process_headers()
{
	std::string							key;
	std::string							val;
	size_t								cursor = 0;
	size_t								found;
	size_t								delim;

	for (;;)
	{
		found = buffer_recv.find("\r\n", cursor);
		if (found == std::string::npos)
			return HEADERS;
		if (found == cursor)
		{
			if (!host.empty())
				return BODY;
			else
				return ERROR;
		}
		delim = buffer_recv.find(":", cursor);
		/* ignore invalid headers */
		if (delim == std::string::npos || delim > found)
		{
			cursor = found + DELIM;
			continue ;
		}
		key = buffer_recv.substr(cursor, delim - cursor);
		val = buffer_recv.substr(delim + COLON, found - (delim + COLON));
		if (key == "Host")
		{
			/* Host duplicate */
			if (!host.empty())
				return ERROR;
			host = val;
		}
		else if (key == "Content-length")
		{
			/* Content-length duplicate */
			if (content_length != -1)
				return ERROR;
			// try
			content_length = std::stoi(val);
		}
		else
			headers[key] = val;
		cursor = found + DELIM;
	}
}


ssize_t _hexstr_to_int(std::string hexstr) {
	size_t s;   
	if (!isxdigit(hexstr[0]))
		return -1;
	std::stringstream ss;
	ss << std::hex << hexstr;
	ss >> s;
	return s;
}

bool get_simple_body() {
	body.append(buffer_recv.substr(0, _content_length));
	return body.size() == _content_length;
}

bool get_ckunked_body() {
	size_t length = 0;
	std::string const DELIM = "\n";
	size_t pos_delim = 0;
	size_t cursor = 0;
	ssize_t size_chunck = -1;
	while (size_chunck != 0) {
		if ((pos_delim = buffer_recv.find(DELIM, cursor)) == std::string::npos) 
			break;
		// taille
		size_chunck = _hexstr_to_int(buffer_recv.substr(cursor, pos_delim));
		if (size_chunck < 0)
			throw std::logic_error("error chunked : hexa size");
		if (pos_delim + DELIM.size() + size_chunck + DELIM.size() > buffer_recv.size())
			break;
		#ifdef DEBUG1
		std::cout << "-> " << size_chunck << "\n";
		#endif
		cursor = pos_delim + DELIM.size();
		body.append(buffer_recv.substr(cursor, size_chunck));
		#ifdef DEBUG1
		std::cout << " {" <<  buffer_recv.substr(cursor, size_chunck) << "}\n";
		#endif
		cursor += size_chunck;
		if (buffer_recv.compare(cursor, DELIM.size(), DELIM) != 0)
			throw std::logic_error("error chunked : delimiter expected");
		cursor += DELIM.size();
		length += size_chunck;
	}
	buffer_recv.erase(0, cursor);
	_content_length += length;
	return (size_chunck == 0);
}

std::string  _statetostr(e_http_state st) {
	switch (st) {
		case HEADERS:
			return "HEADERS";
		case BODY:
			return "BODY";
		case RESPONSE:
			return "RESPONSE";
		default:
			return "ERROR";
	}
}

e_http_state Request::process_body() {
		if (!_chunked) {
			if (get_simple_body())
				state = RESPONSE;
		} else {
			try {
				if (get_ckunked_body())
					state = RESPONSE;
			} catch(std::logic_error const & e) {
				state = ERROR;
				#ifdef DEBUG
				std::cout << e.what() << "\n";
				#endif
			}
		}
		#ifdef DEBUG
		std::cout << "body :  {" << body << "}\n";
		std::cout << "content-length :  {" << _content_length << "}\n";
		std::cout << "rest : {" << buffer_recv << "}\n" ;
		std::cout << "state: " << _statetostr(state) << "\n";
		#endif
	}
}

