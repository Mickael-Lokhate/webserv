#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include "Route.hpp"
#include "Request.hpp"

class Server
{
public:
	std::string					address;
	std::string					port;
	std::vector<std::string>	server_name;
	std::vector<Route>			routes;
	
public:
	Server();
	~Server();

	void add_route(const Route & route);
	Route choose_route(const Request & req);
	void what();

public:
	void _delete_uri_variable(std::string & loc);
	void _convert_uri_dot(std::string & loc);
	void _remove_simple_dot(std::string & loc);
	void _format_double_dot(std::string & loc);
	void _decode_uri(std::string & loc);
};

template<class T>
T	HexToInt(const std::string &str)
{
	if(str.size()==0)
		return 0;
	std::istringstream iss(str);
	T	result = 0;
	iss >> std::hex >> result;
	return result;
}

std::vector<std::string> split(const std::string & tosplit, char delim);
