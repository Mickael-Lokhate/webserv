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
	try {
	load.add_servers(servers);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
//	load.what();
	return (0);
}
