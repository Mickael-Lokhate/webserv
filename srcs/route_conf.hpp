#pragma once

#include <string>
#include <iostream>

typedef struct s_route_conf
{
	std::string _root;
	std::string _autoindex;
	std::string _max_body_size;
	std::string _location;
	std::string _alias;
	std::string _limit_except;
	std::string _return;
	std::string _ext;
} t_route_conf;