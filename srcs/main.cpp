//#include "Worker.hpp"
//#include "Worker.hpp"
//#include "Socket_server.hpp"
//#include <iostream>
//#include <map>
//#include <vector>
#include "Master.hpp"

int main(int ac, char **av)
{
	(void)ac;
	std::string file;
	if (av[1])
		file = av[1];
	Master master(file);
	master.init();
	master.work();
	/*
	std::map<int, Socket_server> socket_servers;
	std::vector<Socket_server> vsocket; 	

	vsocket.push_back(Socket_server("8080","127.0.0.1"));
	vsocket.push_back(Socket_server("8080","0.0.0.0"));
	
	try {
		for(std::vector<Socket_server>::iterator it = vsocket.begin(); it != vsocket.end(); it++ ) {
			it->bind_();
			it->listen_();
			it->what();
			socket_servers.insert(std::make_pair(it->fd, *it));
		}
	} catch(std::string const & str) {
		std::cout << str <<	"\n";
	}
	Worker mago(socket_servers);
	mago.event_loop();
	*/
}
