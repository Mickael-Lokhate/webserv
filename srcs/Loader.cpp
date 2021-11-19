#include "Loader.hpp:"

Loader::Loader(std::vector<Server> *servers, std::ifstream& config) : _config_file(config)
{
	std::cout << "Loader start" << std::endl;
	_parse_config(servers);
}

Loader::Loader(const Loader& cpy)
{
	std::cout << "Copy of a loader" << std::endl;
}

Loader::~Loader(void)
{
	std::cout << "Loader destruction" << std::endl;
}

Loader&	Loader::operator=(const Loader& rhs)
{
	std::cout << "Assignation of a Loader" << std::endl;
}

void	Loader::_parse_config(std::vector<Servers> *servers)
{
	std::cout << "Configuration parsing start" << std::endl;
	// Parse le fichier de config
	// Appel _add_server quand on rencontre un nouveau server
}

void	Loader::_add_server(std::vector<Servers> *servers)
{
	std::cout << "Adding a server..." << std::endl;
	// Creer un nouvel objet Server
	// Appel _parse_server pour initialiser le nouveau server
	// Ajout du nouveau Server au vecteur
}

void	Loader::_parse_server(Server *new_server)
{
	std::cout << "Parse  of a server start" << std::endl;
	// Setup tous les attributs du server rencontre
	// Appel fonction add_route du server si "Location" est trouve
}
