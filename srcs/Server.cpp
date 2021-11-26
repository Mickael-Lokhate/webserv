#include "Server.hpp"
#include "Request.hpp"
#include <cstring>
#include <sstream>
#include <iomanip>

Server::Server() 
{
	address = "127.0.0.1";
	server_name.push_back("localhost");
	port = "8080";
	Route newroute;
	routes.push_back(newroute);
}

Server::~Server()
{
	;
}

void	Server::add_route(const Route & route)
{
	routes.push_back(route);
}

void	Server::_remove_simple_dot(std::string & loc)
{
	size_t n = 0;
	for (int i = 0; (n = loc.find("./", i)) && n != std::string::npos; i++)
		if ((n - 1 >= 0 && loc[n - 1] != '.'))
			loc.erase(n, 2);
}

void	Server::_format_double_dot(std::string & loc)
{
	size_t n = 0;
	size_t ndot = 0;
	size_t nsave = 0;
	while ((n = loc.find("../")) && n != std::string::npos)
	{
		nsave = n;
		ndot = 2;
		loc.erase(n, 3);
		while ((n = loc.find("../", n)) && n != std::string::npos)
		{
			loc.erase(n, 3);
			ndot++;
		}
		n = nsave;
		n -= 1;
		while (ndot--)
		{
			n = loc.rfind("/", n);
			if (ndot == 0 || n < 0)
			{
				loc.erase(n, nsave - n - 1);
				break;
			}
			n--;
		}
	}
}

void	Server::_convert_uri_dot(std::string & loc)
{
	_remove_simple_dot(loc);
	_format_double_dot(loc);
}

void	Server::_delete_uri_variable(std::string & loc)
{
	size_t n = loc.find('?');
	if (n != std::string::npos)
		loc.erase(n, loc.size() - n);
}

void	Server::_decode_uri(std::string & loc)
{
	std::ostringstream ss;
	std::string buf;
	size_t n = 0;

	while ((n = loc.find('%')) && n != std::string::npos && n < loc.size())
	{
		ss << static_cast<char>(HexToInt<int>(loc.substr(n + 1, 2)));
		loc.erase(n, 3);
		loc.insert(n, ss.str());
		ss.str("");
		ss.clear();
		n++;
	}
}

void	Server::_delete_duplicate_slash(std::string & loc)
{
	size_t n = 0;
	while ((n = loc.find("//")) && n != std::string::npos)
		loc.erase(n, 1);
}

Route	Server::choose_route(const std::string & req)
{
	std::string loc(split(req, ' ').at(1));
	_delete_uri_variable(loc);
	_convert_uri_dot(loc);
	_decode_uri(loc);
	_delete_duplicate_slash(loc);
	std::vector<Route>::iterator it = routes.begin();
	std::vector<Route>::iterator ite = routes.end();
	std::vector<std::string> loc_tk(split(loc, '/'));
	std::vector<std::string>::iterator it_loc;
	std::vector<std::string>::iterator ite_loc;
	std::vector<std::string> routes_tk;
	std::vector<std::string>::iterator it_route;
	std::vector<std::string>::iterator ite_route;
	Route best_match = routes.at(0);
	size_t n_match = 0;
	size_t tmpn_match = 0;
	while (it != ite)
	{
		routes_tk = split(it->location, '/');
		it_route = routes_tk.begin();
		ite_route = routes_tk.end();
		it_loc = loc_tk.begin();
		ite_loc = loc_tk.end();
		tmpn_match = 0;
		// std::cout << "[" << it->ext.empty() << "]" << std::endl;
		std::cout << "[" << (ite_loc - 1)->size() << "]" << std::endl;
		if (it->ext.empty() && (ite_loc - 1)->size() > 3)
			if ((ite_loc - 1)->find(it->ext, (ite_loc - 1)->size() - 3) == std::string::npos)
				continue;
		std::cout << "HERE" << std::endl;
		std::cout << it->location << std::endl;
		while (it_route != ite_route)
		{
			std::cout << "*it_route : " << *it_route << std::endl;
			std::cout << "*it_loc : " << loc_tk.at(1) << std::endl;
			if (*it_route++ == *it_loc++)
				tmpn_match++;
			else
				break;
		}
		if (tmpn_match > n_match)
			best_match = *(it);
		it++;
	}
	// Iterate through routes and wait for the LONGEST uri match paired with the right ext -> atm exact matching
	// if found return the route
	// if not return default route (first route if not set /);
	return *(routes.begin());
}

void	Server::what()
{
	std::cout << "------ Server ------" << std::endl;
	std::vector<Route>::iterator it = routes.begin();
	std::vector<Route>::iterator ite = routes.end();
	std::vector<std::string>::iterator it_serv = server_name.begin();

	std::cout << "address : " << address << std::endl;
	std::cout << "port : " << port << std::endl;
	std::cout << "server_name : " << std::endl;
	for (;it_serv != server_name.end(); ++it_serv)
		std::cout << "	" << *it_serv << std::endl;
	while (it != ite)
	{
		it.base()->what();
		it++;
	}
}
