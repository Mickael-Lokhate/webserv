#pragma once

#include <fstream>
#include <vector>
#include "Server.hpp"

class Loader {

public:
	Loader(std::ifstream& config);
	~Loader(void);
	Loader(const Loader& cpy);

	void	to_string(void);
	void	add_servers(std::vector<Server> *servers);

	Loader&	operator=(const Loader& rhs);
private:
	std::ifstream	_config_file; 

	Loader(void);
	void	_parse_config(std::vector<Server> *servers);
	void	_parse_server(Server *new_server);
}

