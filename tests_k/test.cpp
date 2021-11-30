#include <iostream>
#include <string>
#include "Server.hpp"
#include "Route.hpp"
#include "TesterServer.hpp"

int	main(void)
{
	TesterServer test;

	test.start();

	test.server.choose_route("GET /directory/py HTTP/1.1").what();
}