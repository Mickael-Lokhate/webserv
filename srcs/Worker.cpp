#include "worker.hpp"
#include <algorithm>

template<class T>
void gwhat(T & obj) { obj.what(); }

Worker::Worker(const vector<Server> & server_list,
		const map<Socket, Server> & socket_list) :
	r(server_list),
	s(socket_list)
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
