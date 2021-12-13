#pragma once
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <stdexcept>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include "Socket_server.hpp"
#include "Socket_client.hpp"
#include <signal.h>

class Worker {

	public:

	Worker(const std::map<int, Socket_server> & socket_servers);
	Worker(const Worker & ref);
	~Worker(void);
	Worker & operator=(const Worker & right);
	Socket_client & retrieve_client(long i);

	void event_loop(void);

	private:

	void	register_socket_servers(void);
	void	update_modif_list(int ident, int16_t filter,
				uint16_t flags = 0, uint32_t fflags = 0,
				intptr_t data = 0, void *udata = 0);

	void	new_client(int i);
	void	del_client(int i);

	void	recv_client(int i);
	void	send_client(int i);

	void	process_client(int i);
	void	read_client(int i);
	void	write_client(int i);
	bool 	ignore_event(int i);

	void	abort_write(Socket_client & client);
	
	std::vector<struct kevent>		_event_list;
	std::vector<struct kevent>		_modif_list;
	int								number_of_events;
	std::map<int, Socket_client>	_socket_clients;
	std::map<int, Socket_server>	_socket_servers;
	std::set<int>					_closed_clients;

};
