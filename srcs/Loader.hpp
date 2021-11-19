#pragma once

#include <fstream>
#include <vector>
#include "Server.hpp"
#include "Route.hpp"

class Loader {

public:
	Loader(std::ifstream& config);
	~Loader(void);

	void	what(void) const;
	void	add_servers(std::vector<Server> *servers);
	void	create_route(Server *server);

private:
	std::ifstream	_config_file; 

	Loader(void);
	void	_parse_config(std::vector<Server> *servers);
	void	_parse_server(Server *new_server);
};

