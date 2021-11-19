#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "Route.hpp"
#include "Default_server.hpp"

class Server : public Default_server
{
private:
	std::string _address;
	std::string _port;
	std::string _server_name;
	std::map<std::string, std::string> _error_page;
	std::string _root;
	std::string _autoindex;
	std::string _max_body_size;
	std::vector<Route> _routes;
	
public:
	Server();
	~Server();

	void add_route();
};
