#include "Worker.hpp"
#include <map>
#include <vector>

template<class T>
void gwhat(T & obj) { obj.what(); }

Worker::Worker(const std::vector<Server> & servers, const std::map<int, Socket_server> & socket_servers) :
	_socket_servers(socket_servers),
	_servers(servers)
{
#ifdef DEBUG
	std::cout << "[Worker] - Constructor param" << std::endl;
#endif
}

Worker::Worker(const Worker & ref)
{
#ifdef DEBUG
	std::cout << "[Worker] - Constructor copy" << std::endl;
#endif
	*this = ref;
}

Worker::~Worker(void)
{
#ifdef DEBUG
	std::cout << "[Worker] - Destructor" << std::endl;
#endif
}

Worker & Worker::operator=(const Worker & right) 
{
	_socket_clients = right._socket_clients;
	_socket_servers = right._socket_servers;
#ifdef DEBUG
	std::cout << "[Worker] - operator=" << std::endl;
#endif
	return *this;
}

void Worker::event_loop(void)
{
	;
}

void Worker::what(void) const
{
	//for_each(_socket_clients.begin(), _socket_clients.end(), gwhat);
	//for_each(_socket_servers.begin(), _socket_servers.end(), gwhat);
}
