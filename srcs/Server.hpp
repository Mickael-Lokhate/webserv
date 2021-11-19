#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "Route.hpp"
#include "route_conf.hpp"
#include "Default_server.hpp"

class Server : public Default_server
{
private:
	std::string			_address;
	std::string			_port;
	std::string			_server_name;
	std::map<std::string, std::string> _error_page;
	std::string			_root;
	std::string			_autoindex;
	std::string			_max_body_size;
	std::vector<Route>	_routes;
	Route				_default_route;
	
public:
	Server();
	~Server();

	void add_route(t_route_conf conf);
	Route choose_route(std::string uri, std::string ext = NULL);
	void what() const;


	// setter/getter
};
