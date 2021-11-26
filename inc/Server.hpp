#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include "utils.hpp"
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
	Route choose_route(const std::string & req);
	void what();

public:
	void _delete_uri_variable(std::string & loc);
	void _convert_uri_dot(std::string & loc);
	void _remove_simple_dot(std::string & loc);
	void _format_double_dot(std::string & loc);
	void _decode_uri(std::string & loc);
	void _delete_duplicate_slash(std::string & loc);
};

