#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "Socket_client.hpp"
#define SPACE 1
#define TRAIL 3
#define COLON 1
#define DELIM 2

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

bool Socket_client::process_request_line(void)
{
	static const std::string	version = "HTTP/1.";
	size_t 						found;

	found = buffer_recv.find("\r\n");
	if (found == std::string::npos)
		return false;
	request.method = check_method(buffer_recv);
	if (method == "err" || buffer_recv[method.size()] != ' ')
		return false;
	found = buffer_recv.find(" ", method.size() + SPACE);
	if (found == std::string::npos)
		return false;
	request.uri = buffer_recv.substr(method.size() + SPACE,
		   					found - (method.size() + SPACE));	
	found = buffer_recv.find(version, method.size() + uri.size() + 
							SPACE * 2);
	if ((found == std::string::npos) ||
		/* doest minor version is a digit ? */ 
		!isdigit(buffer_recv[found + version.size()]) || 
		/* [METHOD SPACE URI SPACE HTTP/x.x\r\n */
		((method.size() + uri.size() + version.size() + 
		2 * SPACE + TRAIL) != buffer_recv.size()))
		return false;
#ifdef DEBUG
	std::cout << "URI is [" << uri << "]\n";
#endif
	return true;
}

bool Socket_client::process_headers(void)
{
	std::string							key;
	std::string							val;
	size_t								cursor = 0;
	size_t								found;
	size_t								delim;

	for (;;)
	{
		found = buffer_recv.find("\r\n", cursor);
		if (found == std::string::npos || found == cursor)
			break ;
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
			if (!request.host.empty())
				return false;
			request.host = val;
		}
		else if (key == "Content-length")
		{
			/* Content-length duplicate */
			if (!request.content_length.empty())
				return false;
			request.content_length = val;
		}
		else 
			request.headers[key] = val;
		cursor = found + DELIM;
	}
#ifdef DEBUG
	for (std::map<std::string, std::string>::iterator it = request.headers.begin();
		it != request.headers.end(); it++)
		std::cout << it->first << ":" << it->second << std::endl;
#endif
	if (!host.empty()) 
		return true;
	else 
		return false;
}


#ifdef TEST
int main(void)
{
	std::vector<std::pair<std::string, bool> >ref;
	std::vector<std::pair<std::string, bool> >hef;

	ref.push_back(std::make_pair("GET / HTTP\r\n", false));
	ref.push_back(std::make_pair("GET / HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("GET /aurelien.html HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("PUT / HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("POST / HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("DELETE / HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("DDELETE / HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("/ HTTP\r\n", false));
	ref.push_back(std::make_pair("GET / \r\n", false));
	ref.push_back(std::make_pair("GETTT / \r\n", false));
	ref.push_back(std::make_pair("                      GET \r\n", false));
	ref.push_back(std::make_pair("\r\n", false));
	ref.push_back(std::make_pair("", false));
	ref.push_back(std::make_pair("GET  / HTTP/1.1 \r\n", false));
	ref.push_back(std::make_pair("GET /index.html HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("GET ////// HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("GET HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET/HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET/ HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET/ HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET / HTTP /1.1\r\n", false));
	ref.push_back(std::make_pair("GET / HTTP/ 1.1\r\n", false));
	ref.push_back(std::make_pair("GET / HTTP/1.1 \r\n", false));
	ref.push_back(std::make_pair("GET / GET HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET PUT / HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET / GET GET HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET / HTTP/1.1 HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair(" GET / HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET\r\n", false));
	ref.push_back(std::make_pair(" GET \r\n", false));
	ref.push_back(std::make_pair("GET / GET \r\n", false));
	/* ------------------------- */
	hef.push_back(std::make_pair("BOnjour: aurelien\r\n"
								 "sdfsdfsdf sdsfasdf sdf \r\n"
								 "Host: sdsfasdf sdf \r\n"
								 "Content-length: 4\r\n", true));
	hef.push_back(std::make_pair("Host:\r\n", false));
	hef.push_back(std::make_pair("Host: localhost\r\n"
								 "Content-length: 4\r\n", true));
	hef.push_back(std::make_pair("Hlst: localhost\r\n"
								 "Content-length: 4\r\n", false));
	hef.push_back(std::make_pair("\r\n", false));
	/* Do we need to verify host field while parsing ? */
	hef.push_back(std::make_pair("Host::\r\n", false));
	hef.push_back(std::make_pair(":::::::::::::::::\r\n", false));
	hef.push_back(std::make_pair(":::::::::::::::::\r\n", false));
	hef.push_back(std::make_pair("a:b:c:d:e:f:g:h:i\r\n", false));
	hef.push_back(std::make_pair("Host\r\n: localhost\r\n", false));
	hef.push_back(std::make_pair("\r\nHost: localhost\r\n", false));
	hef.push_back(std::make_pair("Host\r\n", false));

	for (size_t i = 0; i < ref.size(); i++)
	{
		if (process_request_line(ref[i].first) != ref[i].second)
			std::cout << "[KO] - " << ref[i].first;
		else
			std::cout << "[OK] - " << ref[i].first;
	}
	std::cout << "----------------------" << std::endl;
	for (size_t i = 0; i < hef.size(); i++)
	{
		if (process_headers(hef[i].first) != hef[i].second)
			std::cout << "[KO] - " << hef[i].first;
		else
			std::cout << "[OK] - " << hef[i].first;
	}

}
#endif
