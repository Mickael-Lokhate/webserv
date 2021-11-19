#include "Worker.hpp"
#include <map>
#include <vector>
#include <algorithm>

template<class T>
void gwhat(T & obj) { obj.what(); }

Worker(const std::vector<Server> & servers,
		const std::map<int, Socket_server> & socket_servers) :
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
	*this = ref
}

Worker::~Worker(void)
{
#ifdef DEBUG
	std::cout << "[Worker] - Destructor" << std::endl;
#endif
}

Worker & Worker::operator=(const Worker & right) :
	r(right.r),
	s(right.s)
{
#ifdef DEBUG
	std::cout << "[Worker] - operator=" << std::endl;
#endif
}

void Worker::event_loop(void)
{
	;
}

void Worker::what(void) const
{
	foreach(r.begin(), r.end(), gwhat);
	foreach(s.begin(), s.end(), gwhat);
}
