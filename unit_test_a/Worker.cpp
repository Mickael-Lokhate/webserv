#include "Worker.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>

template<class T>
void gwhat(T & obj) { obj.what(); }

Worker::Worker(const std::map<int, Socket_server> & socket_servers) :
	_socket_servers(socket_servers)
{
	std::map<int, Socket_server>::iterator last = _socket_servers.end();
	std::map<int, Socket_server>::iterator it = _socket_servers.begin();

	event_list.resize(_socket_servers.size());
	modif_list.resize(_socket_servers.size());
	for (int i = 0; it != last; it++)
		EV_SET(&(modif_list[i++]), it->second.fd,
				EVFILT_READ, EV_ADD, 0, 0, NULL);
}

Worker::Worker(const Worker & ref)
{
	*this = ref;
}

Worker::~Worker(void)
{
}

Worker & Worker::operator=(const Worker & right) 
{
	event_list = right.event_list;
	modif_list = right.modif_list;
	_socket_clients = right._socket_clients;
	_socket_servers = right._socket_servers;
	return *this;
}

void Worker::update_modif_list(int fd, int16_t filter, uint16_t flags)
{
	modif_list.resize(modif_list.size() + 1);
	EV_SET(&(*(modif_list.end() - 1)), fd, filter, flags, 0, 0, NULL);
}

void Worker::new_client(int i)
{
	int 				new_client;
	struct sockaddr_in	from;
	socklen_t			slen;

	if (event_list[i].flags & EV_EOF)
		/* returns the socket error (if any) in fflags */
		throw std::runtime_error(std::string(strerror(event_list[i].fflags)));
	slen = sizeof(from);
	/* data contains the size of the listen backlog. */
	while (event_list[i].data--)
	{
		if ((new_client = accept(event_list[i].ident,
						(struct sockaddr *)&from, &slen)) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
		update_modif_list(new_client, EVFILT_READ, EV_ADD);
		char buffer[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(from.sin_addr), buffer, INET_ADDRSTRLEN);
		_socket_clients.insert(std::make_pair(new_client,
						Socket_client(new_client, buffer,
						std::to_string(htons(from.sin_port)))));
#ifdef DEBUG
		std::cout << "[Worker] -   new client -> " ;
		_socket_clients.find(new_client)->second.what();
		std::cout << "\n";
#endif
	}
}

void Worker::recv_client(int i)
{
	char	buffer[event_list[i].data];

	if (read(event_list[i].ident, buffer, event_list[i].data) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
#ifdef DEBUG
	std::cout << "[Worker] -  read client -> ";
	_socket_clients.find(event_list[i].ident)->second.what();
	std::cout << ", " << event_list[i].data << " bytes to read\n";
#endif 
	_socket_clients[event_list[i].ident].buff.append(buffer, event_list[i].data);
	update_modif_list(event_list[i].ident, EVFILT_WRITE, EV_ADD);
}

void Worker::send_client(int i)
{
	Socket_client & client = _socket_clients[event_list[i].ident];

	if (write(event_list[i].ident, client.buff.c_str(),
				client.buff.size()) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
#ifdef DEBUG
	std::cout << "[Worker] - write client -> ";
	_socket_clients.find(event_list[i].ident)->second.what();
	std::cout << ", " << client.buff.size() << " bytes to send, ";
	std::cout << event_list[i].data << " bytes in pipe\n";
#endif 
	client.buff.clear();
	update_modif_list(event_list[i].ident, EVFILT_WRITE, EV_DELETE);
}

void Worker::del_client(int i)
{
#ifdef DEBUG
	std::cout << "[Worker] -   fin client -> ";
	_socket_clients.find(event_list[i].ident)->second.what();
	std::cout << "\n";
#endif 
	_socket_clients.erase(event_list[i].ident);
	close(event_list[i].ident);
}

void Worker::event_loop(void)
{
	int				number_of_events;
	int				kq;
	std::map<int, Socket_server>::iterator last = _socket_servers.end();

	if ((kq = kqueue()) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
	while (1)
	{
		if ((number_of_events = kevent(kq, &(*(modif_list.begin())),
			modif_list.size(), &(*(event_list.begin())),
			event_list.size(), NULL)) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
		modif_list.resize(0);
		for (int i = 0; i < number_of_events; i++)
		{
			try 
			{ 
				/* nouvelle connexion client */
				if (_socket_servers.find(event_list[i].ident) != last)
					new_client(i);
				else
				{
					/* fin de connexion client */
					if (event_list[i].flags & EV_EOF)
						del_client(i);
					else
					{
						/* réception client */
						if (event_list[i].filter == EVFILT_READ)
							recv_client(i);
						/* émission client */
						if (event_list[i].filter == EVFILT_WRITE)
							send_client(i);
					}
				}
			}
			catch (std::exception & e)
			{
				std::cerr << "Worker::event_loop: " << e.what() << std::endl;
			}
		}
		/* ? */
		event_list.resize(_socket_clients.size() + _socket_servers.size());
	}

}

void Worker::what(void) const
{
	;
}
