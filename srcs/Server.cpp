#include "../inc/Server.hpp"
#include "../inc/Request.hpp"
#include <cstring>
#include <sstream>

Server::Server() 
{
	address = "127.0.0.1";
	server_name = "localhost";
	port = "8080";
	Route newroute;
	routes.push_back(newroute);
}

Server::~Server()
{
	;
}

std::vector<std::string> split(const std::string & tosplit, char delim)
{
	std::string buf;
	std::stringstream ss(tosplit);

	std::vector<std::string> tokens;

	while (getline(ss, buf, delim))
		tokens.push_back(buf);
	
	return tokens;

}

void	Server::add_route(const Route & route)
{
	routes.push_back(route);
}

Route	Server::choose_route(const Request & req)
{
	std::string loc(split(req.req_line, ' ').at(1));
	std::vector<Route>::iterator it = routes.begin();
	std::vector<Route>::iterator ite = routes.end();
	while (it != ite)
	{
		if (!(it.base()->location.compare(loc)))
			return *it;
		it++;
	}
	// Iterate through routes and wait for the LONGEST uri match paired with the right ext -> atm exact matching
	// if found return the route
	// if not return default route (first route if not set /);
	return *(routes.begin());
}

void Server::what()
{
	std::cout << "------ Server ------" << std::endl;
	std::vector<Route>::iterator it = routes.begin();
	std::vector<Route>::iterator ite = routes.end();


	std::cout << "address : " << address << std::endl;
	std::cout << "port : " << port << std::endl;
	std::cout << "server_name : " << server_name << std::endl;
	
	while (it != ite)
	{
		it.base()->what();
		it++;
	}
}
