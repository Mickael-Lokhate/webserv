#include "Worker.hpp"
#include <stdexcept>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>

template<class T>
void gwhat(T & obj) { obj.what(); }

Worker::Worker(const std::vector<Server> & servers,
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

void Worker::new_client(int kq, struct kevent & event)
{
	int 				new_client;
	struct sockaddr_in	from;
	socklen_t			slen;
	struct kevent		add_event;

	/* 
	 * On accept(2) le(s) nouveau(x) client(s) et on le(s) 
	 * ajoute a notre kevent, si une erreure survient, on 
	 * ne la considére pas comme critique et on fait fi de 
	 * cette erreure pour continer l'éxecution
	 */
	if (event.flags & EV_EOF)
		/* returns the socket error (if any) in fflags */
		throw std::runtime_error(std::string(strerror(event.fflags)));
	slen = sizeof(slen);
	/* data contains the size of the listen backlog. */
	while (event.data--)
	{
		if ((new_client = accept(event.ident,
						(struct sockaddr *)&from, &slen)) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
		EV_SET(&add_event, new_client, EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(kq, &add_event, 1, NULL, 0, NULL) == -1)
		{
			close(new_client);
			throw std::runtime_error(std::string(strerror(errno)));
		}
		_socket_clients.insert(std::make_pair(new_client,
							Socket_client(new_client)));
	}
}

void Worker::recv_client(int kq, struct kevent & event)
{
}

void Worker::send_client(int kq, struct kevent & event)
{
}

void Worker::del_client(int kq, struct kevent & event)
{
	struct kevent		del_event;

	EV_SET(&del_event, event.ident, 0, EV_DELETE, 0, 0, NULL);
	if (kevent(kq, &del_event, 1, NULL, 0, NULL) == -1)
	{
		/* est-il vraiment utile de le supprimmer de la map ? */
		_socket_clients.erase(event.ident);
		close(event.ident);
		throw std::runtime_error(std::string(strerror(errno)));
	}
	/* est-il vraiment utile de le supprimmer de la map ? */
	_socket_clients.erase(event.ident);
	close(event.ident);
}

void Worker::event_loop(void)
{
	struct kevent	eventlist[MAX_EVENTS];
	struct kevent	event;
	int				kq;
	int				number_of_events;
	std::map<int, Socket_server>::iterator last;

	if ((kq = kqueue()) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
	for (std::map<int, Socket_server>::iterator it = _socket_servers.begin(),
			last = _socket_servers.end(); it != last; it++)
	{
		/* 
		 * On ajoute chaque listener a la klist, on pourrait optimiser
		 * ce code en ne faisant qu'un seul ajout avec tous les sockets 
		 * et ainsi éviter la boucle
		 */
		EV_SET(&event, it->fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
	}
	while (1)
	{
		/* 
		 * Boucle principale d'écoute, on récupére les événements
		 * et on effectue le traitement en fonction du filtre levé
		 */
		if ((number_of_events = kevent(kq, NULL, 0,
				eventlist, MAX_EVENTS, NULL)) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
		for (int i = 0; i < number_of_events; i++)
		{
			try 
			{ 
				/* nouvelle connexion client */
				if (_socket_servers.find(eventlist[i].ident) != last)
					new_client(kq, eventlist[i]);
				else
				{
					/* fin de connexion client */
					if (eventlist[i].flags & EV_EOF)
						del_client(kq, eventlist[i]);
					else
					{
						/* réception client */
						if (eventlist[i].filter & EVFILT_READ)
							recv_client(kq, eventlist[i]);
						/* émission client */
						if (eventlist[i].filter & EVFILT_WRITE)
							send_client(kq, eventlist[i]);
					}
				}
			}
			catch (std::exception & e)
			{
				std::cerr << "Worker::event_loop: " << e.what() << std::endl;
			}
		}
	}

}

void Worker::what(void) const
{
	//for_each(_socket_clients.begin(), _socket_clients.end(), gwhat);
	//for_each(_socket_servers.begin(), _socket_servers.end(), gwhat);
}
