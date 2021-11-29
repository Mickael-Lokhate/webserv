#include <iostream>
#include <string>
#include "Server.hpp"
#include "Route.hpp"

int	main(void)
{
	Server server;
	std::string loc = "/example/sousdoc/////.././.././%4F/%20/%42/example2?test=2";

	std::cout << loc << std::endl;
	std::cout << "------- delete_uri_variable" << std::endl;
	server._delete_uri_variable(loc);
	std::cout << loc << std::endl;
	std::cout << "------- remove_simple_dot" << std::endl;
	server._remove_simple_dot(loc);
	std::cout << loc << std::endl;
	std::cout << "------- format_double_dot" << std::endl;
	server._format_double_dot(loc);
	std::cout << loc << std::endl;
	std::cout << "------- decode_uri" << std::endl;
	server._decode_uri(loc);
	std::cout << loc << std::endl;
	std::cout << "------- delete_duplicate_slash" << std::endl;
	server._delete_duplicate_slash(loc);
	std::cout << loc << std::endl;

	Route directory;
	directory.location = "directory/";
	Route directoryphp;
	directoryphp.location = "directory/";
	directoryphp.ext = "php";
	Route directorypy;
	directorypy.location = "directory/";
	directorypy.ext = "py";
	Route directorytest;
	directorytest.location = "directory/test";
	server.add_route(directory);
	server.add_route(directoryphp);
	server.add_route(directorypy);
	server.add_route(directorytest);

	server.choose_route("GET /directory/py HTTP/1.1").what();
}