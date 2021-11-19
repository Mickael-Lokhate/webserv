#pragma once

#include <string>
#include <iostream>
#include "Route.hpp"

class Route
{
private:
	std::string _root;
	std::string _autoindex;
	std::string _max_body_size;
	std::string _location;
	std::string _alias;
	std::string _limit_except;
	std::string _return;
	std::string _ext;

	
public:
	Route();
	~Route();
};
