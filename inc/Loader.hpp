#pragma once

#include <fstream>
#include <vector>
#include <algorithm>
#include "Server.hpp"
#include "Route.hpp"

class Loader {

public:
	Loader(std::ifstream & config);
	~Loader(void);

	void	what(void) const;
	void	add_servers(std::vector<Server> & servers);

private:
	std::ifstream & _config_file; 
	std::vector<std::string> _config_tab;

	Loader(void);
	void	_parse_config(std::vector<Server> & servers);
	void	_parse_server(Server & new_server);
	void	_print_config(std::string curr_line) const;
	void	_create_route(Server & server, Route &default_route, std::vector<std::vector<std::string>::iterator>& loc_it);
	std::string	_trim(const std::string &s);
	std::string	_ltrim(const std::string &s);
	std::string	_rtrim(const std::string &s);
};

