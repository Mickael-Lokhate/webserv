#include "Worker.hpp"
#include <limits.h>

template<class T>
void gwhat(T & obj) { obj.what(); }

Worker::Worker(const std::map<int, Socket_server> & socket_servers) :
	_socket_servers(socket_servers)
{
	std::map<int, Socket_server>::iterator last = _socket_servers.end();
	std::map<int, Socket_server>::iterator it = _socket_servers.begin();

	/* resize vectors for first kevent loop */
	_event_list.resize(_socket_servers.size());
	_modif_list.resize(_socket_servers.size());
	/* register listeners */
	for (int i = 0; it != last; it++)
		EV_SET(&(_modif_list[i++]), it->second.fd,
				EVFILT_READ, EV_ADD, 0, 0, NULL);
}

Worker::Worker(const Worker & ref)
{
	*this = ref;
}

Worker::~Worker(void)
{
	;
}

Worker & Worker::operator=(const Worker & right) 
{
	_event_list = right._event_list;
	_modif_list = right._modif_list;
	_socket_clients = right._socket_clients;
	_socket_servers = right._socket_servers;
	_closed_clients = right._closed_clients;
	return *this;
}

void Worker::update_modif_list(int ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent event;

	EV_SET(&event, ident, filter, flags, fflags, data, udata);
	_modif_list.push_back(event);
}

void Worker::new_client(int i)
{
	int 				new_client;
	struct sockaddr_in	from;
	socklen_t			slen;

	if (_event_list[i].flags & EV_EOF)
		/* returns the socket error (if any) in fflags */
		throw std::runtime_error(std::string(strerror(_event_list[i].fflags)));
	slen = sizeof(from);
	/* data contains the size of the listen backlog. */
	while (_event_list[i].data--)
	{
		if ((new_client = accept(_event_list[i].ident,
						(struct sockaddr *)&from, &slen)) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
		/* Setup client's timeout for request line and headers */
		update_modif_list(new_client, EVFILT_TIMER,
			EV_ADD | EV_ONESHOT, NOTE_SECONDS, TO_HEADERS);
		update_modif_list(new_client, EVFILT_READ, EV_ADD);
		char buffer[INET_ADDRSTRLEN];
		/* Store client's addr and port as string */
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
	char			buffer[_event_list[i].data];
	Socket_client & client = _socket_clients[_event_list[i].ident];

	if (read(_event_list[i].ident, buffer, _event_list[i].data) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
#ifdef DEBUG
	std::cout << "[Worker] -  recv client -> ";
	_socket_clients.find(_event_list[i].ident)->second.what();
	std::cout << ", " << _event_list[i].data << " bytes to read";
	std::cout << "\n";
#endif 
	client.buffer_recv.append(buffer, _event_list[i].data);
	/* After receiving data, we register a user event to wakeup process_client */
	if (client.state < RESPONSE)
		update_modif_list(client.fd, EVFILT_USER,
			EV_ADD | EV_ONESHOT, NOTE_TRIGGER);

}

void Worker::send_client(int i)
{
	Socket_client & client = _socket_clients[_event_list[i].ident];
	ssize_t size_send;

	#ifdef DEBUG
	std::cout << "[Worker] - send client -> ";
	_socket_clients.find(_event_list[i].ident)->second.what();
	std::cout << ", " << client.buffer_send.size() << " bytes to send, ";
	std::cout << _event_list[i].data << " bytes in pipe";
	std::cout << "\n";
	client.request.what();
	#endif
	size_send = std::min(client.buffer_send.size(), (size_t)_event_list[i].data);
	size_send = write(_event_list[i].ident, client.buffer_send.c_str(), size_send);
	if (size_send == -1)
		throw std::runtime_error(std::string(strerror(errno)));
	if ((size_t)size_send < client.buffer_send.size())
	{
		update_modif_list(client.fd, EVFILT_TIMER,
			EV_ADD | EV_ONESHOT, NOTE_SECONDS, TO_SEND);
		client.buffer_send.erase(0, size_send);
	}
	else 
	{
		if (client.request.error == 400 || client.request.error == 501)
			del_client(i);
		else {
			client.buffer_send.clear();
			client.state = REQUEST_LINE;
			client.request = Request(&client.buffer_recv);
			update_modif_list(_event_list[i].ident, EVFILT_WRITE, EV_DELETE);
			/* Setup client's timeout for sending back data */
			update_modif_list(client.fd, EVFILT_TIMER,
					EV_ADD | EV_ONESHOT, NOTE_SECONDS, TO_HEADERS);
			std::cout << "{" << client.buffer_recv << "}\n";
			if (!client.buffer_recv.empty()) 
			{
				/* Reset client's state to process another waiting request */
				update_modif_list(client.fd, EVFILT_USER,
					EV_ADD | EV_ONESHOT, NOTE_TRIGGER);
			}
		}
	}
}

void Worker::del_client(int i)
{
#ifdef DEBUG
	std::cout << "[Worker] -   " << 
	((_event_list[i].filter == EVFILT_TIMER) ? "TO" : "fin") 
	<< " client -> ";
	_socket_clients.find(_event_list[i].ident)->second.what();
	std::cout << "\n";
#endif 
	/* Delete timeout if it was not triggered */
	if (_event_list[i].filter != EVFILT_TIMER)
		update_modif_list(_event_list[i].ident, EVFILT_TIMER, EV_DELETE);
	close(_event_list[i].ident);
	_socket_clients.erase(_event_list[i].ident);
	_closed_clients.insert(_event_list[i].ident);
}

void Worker::read_client(int i)
{
	int fd_file = _event_list[i].ident;
	Socket_client & client = _socket_clients[(long)_event_list[i].udata];
#ifdef DEBUG
	std::cout << "[Worker] -   Read client -> ";
	client.what();
	std::cout << "\n";
#endif 

	char buffer[_event_list[i].data];

	read(fd_file, buffer, _event_list[i].data);
	client.buffer_send.append(buffer, _event_list[i].data);
}

// upload 
void Worker::write_client(int i)
{
	(void)i;
}

void Worker::process_client(int i)
{

	Socket_client & client = _socket_clients[_event_list[i].ident];

	#ifdef DEBUG
		std::cout << "[Worker] -   hundle client -> ";
		_socket_clients[client.fd].what();
		std::cout << "\n";
	#endif 
	if (client.state < RESPONSE) 
	{
		if (state == REQUEST_LINE)
			client.process_request_line();
		if (state == HEADERS)
			client.process_headers();
		if (client.state == ROUTE)
			client.get_root();
		if (client.state == BODY) 
		{
			update_modif_list(client.fd, EVFILT_TIMER,
				EV_ADD, NOTE_SECONDS, TO_BODY);
			client.process_body();
		}
	}
	if (client.state >= RESPONSE) 
	{
		if (client.state == RESPONSE)
		{	
			update_modif_list(client.fd, EVFILT_TIMER,
					EV_ADD, NOTE_SECONDS, TO_RESPONSE);
			if (client.response.status == 400 || client.response.status == 501)
				update_modif_list(_event_list[i].ident, EVFILT_READ, EV_DELETE);
		}
		client.process_response();
		if (client.state == NEED_READ)
			update_modif_list(client.fd_read, EVFILT_READ, EV_ADD,
				0, 0,(void *)((long)client.fd));
		if (client.state == NEED_WRITE)
			update_modif_list(client.fd_write, EVFILT_READ, EV_ADD,
					0, 0,(void *)((long)client.fd));
		if(client.state == READY)
			update_modif_list(client.fd, EVFILT_WRITE, EV_ADD);
	}
}

void Worker::event_loop(void)
{
	int					number_of_events;
	int					kq;
	std::map<int, Socket_server>::iterator last = _socket_servers.end();

	if ((kq = kqueue()) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
	while (1)
	{
		if ((number_of_events = kevent(kq, _modif_list.begin().base(),
			_modif_list.size(), _event_list.begin().base(),
			_event_list.size(), NULL)) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
		_modif_list.resize(0);
		_closed_clients.clear();
		std::cout << "-----\n" ;
		for (int i = 0; i < number_of_events; i++)
		{
			try 
			{ 
				/* nouvelle connexion client */
				if (_socket_servers.find(_event_list[i].ident) != last)
					new_client(i);
				else
				{
					if (_closed_clients.find(_event_list[i].ident) != 
						_closed_clients.end())
						continue ;
					/* fin de connexion client */
					else if  (_event_list[i].flags & EV_EOF || 
							_event_list[i].filter == EVFILT_TIMER)
						del_client(i);
					else
					{
						/* réception client */
						if (_event_list[i].filter == EVFILT_READ)
						{
							if (_event_list[i].udata)
								read_client(i);
							else
								recv_client(i);
						}
						/* émission client */
						else if (_event_list[i].filter == EVFILT_WRITE)
						{
							if (_event_list[i].udata)
								write_client(i);
							else
								send_client(i);
						}
						else if (_event_list[i].filter == EVFILT_USER)
							process_client(i);
					}
				}
			}
			catch (std::exception & e)
			{
				std::cerr << "Worker::event_loop: " << e.what() << std::endl;
			}
		}
		_event_list.resize(_socket_clients.size() +
							+ _modif_list.size() + _socket_servers.size());
	}

}
