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
	_socket_clients = right._socket_clients;
	_socket_servers = right._socket_servers;
	return *this;
}

void Worker::new_client(int i)
{
	int 				new_client;
	struct sockaddr_in	from;
	socklen_t			slen;

	/* 
	 * On accept(2) le(s) nouveau(x) client(s) et on le(s) 
	 * ajoute a notre kevent, si une erreure survient, on 
	 * ne la considére pas comme critique et on fait fi de 
	 * cette erreure pour continer l'éxecution
	 */
#ifdef DEBUG
	std::cout << "[Worker] - new client" << std::endl;
#endif 
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
		modif_list.resize(modif_list.size() + 1);
		EV_SET(&(*(modif_list.end() - 1)), new_client,
				EVFILT_READ, EV_ADD, 0, 0, NULL);
		char buffer[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(from.sin_addr), buffer, INET_ADDRSTRLEN);
		_socket_clients.insert(std::make_pair(new_client,
						Socket_client(new_client, buffer,
						std::to_string(htons(from.sin_port)))));
#ifdef DEBUG
		_socket_clients.find(new_client)->second.what();
#endif
	}
}

void Worker::recv_client(int i)
{
	char			buffer[event_list[i].data];

#ifdef DEBUG
	std::cout << "[Worker] - in client" << std::endl;
	std::cout << event_list[i].data << " bytes to read" << std::endl;
#endif 
	if (read(event_list[i].ident, buffer, event_list[i].data) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
	_socket_clients[event_list[i].ident].buff.append(buffer, event_list[i].data);
	modif_list.resize(modif_list.size() + 1);
	EV_SET(&(*(modif_list.end() - 1)), event_list[i].ident,
			EVFILT_WRITE, EV_ADD, 0, 0, NULL);
}

void Worker::send_client(int i)
{
	Socket_client & client = _socket_clients[event_list[i].ident];

#ifdef DEBUG
	std::cout << "[Worker] - out client" << std::endl;
	std::cout << client.buff.size() <<
			" bytes to send" << std::endl;
	std::cout << event_list[i].data << " bytes in pipe" << std::endl;
#endif 
	if (write(event_list[i].ident, client.buff.c_str(),
				client.buff.size()) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
	client.buff.clear();
	modif_list.resize(modif_list.size() + 1);
	EV_SET(&(*(modif_list.end() - 1)), event_list[i].ident,
			EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
}

void Worker::del_client(int i)
{
#ifdef DEBUG
	std::cout << "[Worker] - fin client" << std::endl;
#endif 
	/* Nous ne supprimons que le filtre READ, cependant, il 
	 * est possible qu'un send soit en attente pour ce meme
	 * client. Il serait judicieux d'intégrer l'etat de la
	 * connexion dans la classe Socket_client pour pouvoir
	 * supprimer le filtre WRITE quand cela est nécessaire
	 */
	modif_list.resize(modif_list.size() + 1);
	EV_SET(&(*(modif_list.end() - 1)), event_list[i].ident,
			EVFILT_READ, EV_DELETE, 0, 0, NULL);
	_socket_clients.erase(event_list[i].ident);
	close(event_list[i].ident);
}

void Worker::event_loop(void)
{
	int				i = 0;
	int				kq;
	int				number_of_events;
	std::map<int, Socket_server>::iterator last = _socket_servers.end();

	if ((kq = kqueue()) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
	event_list.resize(_socket_servers.size());
	modif_list.resize(_socket_servers.size());
	for (std::map<int, Socket_server>::iterator it = _socket_servers.begin();
		it != last; it++)
	{
		EV_SET(&(modif_list[i++]), it->second.fd,
				EVFILT_READ, EV_ADD, 0, 0, NULL);
	}
	while (1)
	{
		/* 
		 * Boucle principale d'écoute, on récupére les événements
		 * et on effectue le traitement en fonction du filtre levé
		 */
		if ((number_of_events = kevent(kq, &(*(modif_list.begin())),
			modif_list.size(), &(*(event_list.begin())),
			event_list.size(), NULL)) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
		this->modif_list.clear();
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
		this->event_list.resize(this->_socket_clients.size()
								+ this->_socket_servers.size());
	}

}

void Worker::what(void) const
{
	//for_each(_socket_clients.begin(), _socket_clients.end(), gwhat);
	//for_each(_socket_servers.begin(), _socket_servers.end(), gwhat);
}
