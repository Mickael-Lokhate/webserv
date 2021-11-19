#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "Route.hpp"
#include "Request.hpp"
#include "route_conf.hpp"

class Server
{
public:
	std::string			address;
	std::string			port;
	std::string			server_name;
	std::vector<Route>	routes;
	
public:
	Server();
	~Server();

	void add_route(const Route & route);
	Route choose_route(const Request & req);
	void what() const;

	// setter/getter
};
