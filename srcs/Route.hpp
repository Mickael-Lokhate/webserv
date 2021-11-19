#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "Route.hpp"
#include "route_conf.hpp"

class Route
{
public:
	std::pair<std::string, bool> root;
	std::string autoindex;
	std::string max_body_size;
	std::string location;
	std::string limit_except;
	std::map<std::string, std::string> error_page;
	std::string return_;
	std::string ext;
	std::vector<std::string> index;
	std::string cgi;
	std::string upload;

public:
	Route();
	~Route();
	void what() const;

	// setter/getter
};
