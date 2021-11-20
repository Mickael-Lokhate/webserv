#include "Server.hpp"

Server::Server() 
{
	;
}

Server::~Server()
{
	;
}

void	Server::add_route(const Route & conf)
{
	// Create a t_route_conf $conf_serv based on Server attributes
	// Instanciate a route object with $conf_serv
	// Use conf to update the values retrieved from parsing
	// Add the new route to vector
	;
}

Route	Server::choose_route(const Request & req)
{
	// Iterate through routes and wait for the LONGEST uri match paired with the right ext
	// if found return the route
	// if not return default route (first route if not set /);
	;
}

void Server::what() const
{
	// Display all attributes in a formatted view
	;
}
