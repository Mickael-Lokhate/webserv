#include "Loader.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
		return (1);
	std::ifstream	file(av[1], std::ifstream::in);	
	std::vector<Server>	servers;
	
	if (!(file.is_open()))
		return (1);

	Loader load(file);
	load.add_servers(servers);
	load.what();
	return (0);
}
