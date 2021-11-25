#include "Server.hpp"
#include "Request.hpp"
#include <cstring>
#include <sstream>
#include <iomanip>

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

Route	Server::choose_route(const Request & req)
{
	std::string loc(split(req.req_line, ' ').at(1));
	_delete_uri_variable(loc);
	_convert_uri_dot(loc);
	_decode_uri(loc);
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

void	Server::what()
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
