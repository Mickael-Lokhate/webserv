#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include "Request.hpp"

#define SPACE 1
#define COLON 1

Request::Request(std::string * buffer_recv) : 
	delim("\r\n"),
	buffer_recv(buffer_recv) 
{
	//std::cout << " Const @" << buffer_recv << "\n";
}

Request::Request(Request & ref) : 
	delim(ref.delim),
	buffer_recv(ref.buffer_recv) 
{
//	std::cout << " cpy req @" << buffer_recv << "\n";
}

Request & Request::operator=(const Request & ref) {
	delim = ref.delim;
	buffer_recv = ref.buffer_recv;
	return *this;
}

void Request::mytolower(std::string & str)
{
	for (size_t i = 0; i < str.size(); i++)
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] = str[i] + 32;
}

const std::string & Request::check_method() {
	static const std::string	methods[] = {
		"GET", "POST", "PUT", "DELETE", "err"};
	int i;

	for (i = 0; i < 4; i++)
		if (!buffer_recv->compare(0, methods[i].size(),
								methods[i]))
			break;
	return (methods[i]);
}

e_http_state Request::process_request_line()
{
	static const std::string	version = "HTTP/1.";
	size_t 						found;

	//std::cout << "After @" << buffer_recv << "\n";
	found = buffer_recv->find("\n");
	if (found == std::string::npos)
		return REQUEST_LINE;
	if (found && (*buffer_recv)[found - 1] != '\r')
		delim = "\n";
	method = check_method();
	if (method == "err" || (*buffer_recv)[method.size()] != ' ')
		return ERROR;
	found = buffer_recv->find(" ", method.size() + SPACE);
	if (found == std::string::npos)
		return ERROR;
	uri = buffer_recv->substr(method.size() + SPACE,
		   					found - (method.size() + SPACE));
	found = buffer_recv->find(version, method.size() + uri.size() +
							SPACE * 2);
	if ((found == std::string::npos) ||
		/* doest minor version is a digit ? */
		!isdigit((*buffer_recv)[found + version.size()]) ||
		buffer_recv->compare(found + version.size() + SPACE, 
		delim.size(), delim))
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
	size_t								colon;

	for (;;)
	{
		found = buffer_recv->find(delim, cursor);
		if (found == std::string::npos)
			return HEADERS;
		if (found == cursor)
		{
			if (!host.empty())
			{
				buffer_recv->erase(0, found + delim.size());
				return BODY;
			}
			else
				return ERROR;
		}
		colon = buffer_recv->find(":", cursor);
		/* ignore invalid headers */
		if (colon == std::string::npos || colon > found)
		{
			cursor = found + delim.size();
			continue ;
		}
		key = buffer_recv->substr(cursor, colon - cursor);
		mytolower(key);
		val = buffer_recv->substr(colon + COLON,
				found - (colon + COLON));
		val = _ltrim(_rtrim(val));
		if (key == "host")
		{
			/* Host duplicate */
			if (!host.empty())
				return ERROR;
			host = val;
		}
		else if (key == "content-length")
		{
			/* Content-length duplicate */
			if (content_length != -1)
				return ERROR;
			try { content_length = std::stoi(val); }
			catch (std::exception & e)
			{
				return ERROR;
			}
		}
		else if (key == "transfer-encoding")
		{
			/* Transfer-encoding duplicate */
			if (headers.find(key) != headers.end())
				return ERROR;
			if (val == "chunked")
				chunked = true;
			else 
				return ERROR;
		}
		else
			headers[key] = val;
		cursor = found + delim.size();
	}
}


ssize_t Request::_hexstr_to_int(std::string const & hexstr) {
	size_t s;   
	if (!isxdigit(hexstr[0]))
		return -1;
	std::stringstream ss;
	ss << std::hex << hexstr;
	ss >> s;
	return s;
}

bool Request::get_simple_body() {
	body.append(buffer_recv->substr(0, content_length));
	return (body.size() == (size_t)content_length);
}

bool Request::get_ckunked_body() {
	size_t length = 0;
	size_t pos_delim = 0;
	size_t cursor = 0;
	ssize_t size_chunck = -1;
	while (size_chunck != 0) {
		if ((pos_delim = buffer_recv->find(delim, cursor)) == std::string::npos) 
			break;
		// taille
		size_chunck = _hexstr_to_int(buffer_recv->substr(cursor, pos_delim));
		if (size_chunck < 0)
			throw std::logic_error("error chunked : hexa size");
		if (pos_delim + delim.size() + size_chunck + delim.size() > buffer_recv->size())
			break;
		#ifdef DEBUG1
		std::cout << "-> " << size_chunck << "\n";
		#endif
		cursor = pos_delim + delim.size();
		body.append(buffer_recv->substr(cursor, size_chunck));
		#ifdef DEBUG1
		std::cout << " {" <<  buffer_recv->substr(cursor, size_chunck) << "}\n";
		#endif
		cursor += size_chunck;
		if (buffer_recv->compare(cursor, delim.size(), delim) != 0)
			throw std::logic_error("error chunked : delimiter expected");
		cursor += delim.size();
		length += size_chunck;
	}
	buffer_recv->erase(0, cursor);
	content_length += length;
	return (size_chunck == 0);
}

std::string  Request::_statetostr(e_http_state st) {
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
	if (!chunked) {
		if (get_simple_body())
			return RESPONSE;
	} else {
		try {
			if (get_ckunked_body())
				return RESPONSE;
		} catch(std::logic_error const & e) {
			#ifdef DEBUG
			std::cout << "body :  {" << body << "}\n";
			std::cout << "content-length :  {" << content_length << "}\n";
			std::cout << "rest : {" << buffer_recv << "}\n" ;
			std::cout << e.what() << "\n";
			#endif
			return  ERROR;
		}
	}
	return BODY;
}

