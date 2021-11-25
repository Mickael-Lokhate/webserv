#include <iostream>
#include <string>
#include "Server.hpp"
#include "Route.hpp"

int	main(void)
{
	Server server;
	std::string loc = "/example/sousdoc/.././.././example2?test=2";

	server._delete_uri_variable(loc);
	std::cout << loc << std::endl;
	server._remove_simple_dot(loc);
	std::cout << loc << std::endl;
	server._format_double_dot(loc);
	std::cout << loc << std::endl;
}