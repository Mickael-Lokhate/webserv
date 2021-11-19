#include "Loader.hpp"

Loader::Loader(std::ifstream& config) : _config_file(config)
{
	std::cout << "Loader start" << std::endl;
}

Loader::~Loader(void)
{
	std::cout << "Loader destruction" << std::endl;
}

void	Loader::add_servers(std::vector<Server> *servers)
{
	_parse_config(servers);
}

void	Loader::_parse_config(std::vector<Server> *servers)
{
	std::cout << "Configuration parsing start" << std::endl;
	// Parse le fichier de config
	// Bloc server trouve
	Server	new_server;
	_parse_server(&new_server);
	create_route(&new_server);
	// Ajout du serveur au vecteur
}

void	Loader::_parse_server(Server *new_server)
{
	std::cout << "Parse  of a server start" << std::endl;
	// Setup tous les attributs du server rencontre
	// Setup les attributs de new_route_conf avec les valeurs du fichier de conf ou NULL 
}

void	Loader::what(void) const
{
	std::cout << "Loader content : " << std::endl;
	std::cout << _config_file << std::endl;
}

void	Loader::create_route(Server *server)
{
	Route new_route;
	//INitialize route
	server->add_route(new_route);
}