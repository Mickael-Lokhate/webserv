#include "Worker.hpp"
#include <limits.h>

template<class T>
void gwhat(T & obj) { obj.what(); }

Socket_client & Worker::retrieve_client(long i)
{
	std::map<int, Socket_client>::iterator it = _socket_clients.find(i);
	if (it == _socket_clients.end())
		throw std::runtime_error("invalid client");
	return it->second;
}

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
	int					max_back = 32;
	
	if (_event_list[i].flags & EV_EOF)
		/* returns the socket error (if any) in fflags */
		throw std::runtime_error(std::string(strerror(_event_list[i].fflags)));
	slen = sizeof(from);
#ifdef DEBUG
	std::cout << "[Worker] -   Backlog : " << _event_list[i].data << "\n";
#endif
	/* data contains the size of the listen backlog. */
	while (_event_list[i].data-- && max_back--)
	{
		if ((new_client = accept(_event_list[i].ident,
						(struct sockaddr *)&from, &slen)) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
		/* Setup client's timeout for request line and headers */
		update_modif_list(new_client, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TO_HEADERS);
		update_modif_list(new_client, EVFILT_READ, EV_ADD);
		/* Store client's addr and port as string */
		char		buffer[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(from.sin_addr), buffer, INET_ADDRSTRLEN);
		uint16_t 	port = htons(from.sin_port);
		Socket_client client = Socket_client(new_client, buffer,
						to_string(port), 
						&(_socket_servers.find(_event_list[i].ident)->second));
		_socket_clients.insert(std::make_pair(new_client, client));

#ifdef DEBUG
		std::cout << "[Worker] -   new client -> " ;
		_socket_clients.find(new_client)->second.what();
		std::cout << "\n";
#endif
	}
}

void Worker::recv_client(int i)
{
#ifdef DEBUG
	std::cout << "[Worker] -  recv client : \n";
#endif 
	char			buffer[_event_list[i].data];
	Socket_client & client = retrieve_client(_event_list[i].ident);
	if (read(client.fd, buffer, _event_list[i].data) == -1)
	{
		del_client(i);
		throw std::runtime_error(std::string(strerror(errno)));
	}
#ifdef DEBUG
	client.what();
	std::cout << ", " << _event_list[i].data << " bytes to read";
	std::cout << "\n";
#endif 
	client.buffer_recv.append(buffer, _event_list[i].data);
	/* After receiving data, we register a user event to wakeup process_client */
	if (!(client.state & RESPONSE) && !(client.state & READY))
		process_client(client.fd);
}


void Worker::send_client(int i)
{
#ifdef DEBUG
	std::cout << "[Worker] -  send client : \n";
#endif 
	ssize_t			size_send;
	Socket_client & client = retrieve_client(_event_list[i].ident);
	if (client.fd == -1)
		std::cout << "error" << std::endl;

#ifdef DEBUG
	client.what();
	std::cout << ", " << client.buffer_send.size() << " bytes to send, ";
	std::cout << _event_list[i].data << " bytes in pipe";
	std::cout << "\n";
#endif
	size_send = std::min((size_t)_event_list[i].data, client.buffer_send.size());
	size_send = write(client.fd, client.buffer_send.c_str(), size_send);
	if (size_send == -1)
	{
		del_client(i);
		throw std::runtime_error(std::string(strerror(errno)));
	}
	update_modif_list(client.fd, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TO_SEND);
	client.buffer_send.erase(0, size_send);
	if (client.buffer_send.empty() && client.response.read_end)
	{
		if (client.closed)
			del_client(i);
		else {
			update_modif_list(client.fd, EVFILT_WRITE, EV_DELETE | EV_CLEAR);
			client.clean();
			/* Setup client's timeout for sending back data */
			update_modif_list(client.fd, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TO_HEADERS);
			if (!client.buffer_recv.empty()) 
				process_client(client.fd);
		}
	}
}

void Worker::del_client(int i)
{
#ifdef DEBUG
	std::cout << "[Worker] -   " << 
	((_event_list[i].filter == EVFILT_TIMER) ? "TO" : "del") 
	<< " client : \n";
#endif
	Socket_client & client = retrieve_client(_event_list[i].ident);
	if (client.fd == -1)
		std::cout << "error" << std::endl;
		;
#ifdef DEBUG
	client.what();
	std::cout << "\n";
#endif 
	/* Delete timeout if it was not triggered */
	if (_event_list[i].filter != EVFILT_TIMER)
		update_modif_list(client.fd, EVFILT_TIMER, EV_DELETE);
	close(client.fd_read);
	close(client.fd_write);
	close(client.fd);
	_closed_clients.insert(client.fd);
	_socket_clients.erase(client.fd);
}

void Worker::read_client(int i)
{
	int const NB_READ = 128 * 1024;
#ifdef DEBUG
	std::cout << "[Worker] -   Read client : ";
#endif 
	Socket_client & client = retrieve_client((long)_event_list[i].udata);
#ifdef DEBUG
	client.what();
	std::cout << ", " << _event_list[i].data << " bytes to read\n";
#endif 
//	if ((_event_list[i].flags & EV_EOF) && (_event_list[i].data == 0))
//	{
//		close(client.fd_write);
//		close(client.fd_read);
//		client.action = ACTION_NORMAL;
//		client._update_stat(RESPONSE | ERROR, 502);
//		process_client(client.fd);
//		return ;
//	}
	ssize_t nb_read = NB_READ;
	if (client.action == ACTION_CGI)
		nb_read = _event_list[i].data; 	
	char buffer[nb_read];
	/* WE MUST CHECK EV_EOF BEFORE READING */
	ssize_t size_read = read(client.fd_read, buffer, nb_read);

	if (size_read == -1) {
		client._update_stat(RESPONSE | ERROR, 500);
		close(client.fd_read);
		process_client(client.fd);
		return ;	
	}
	client.response.body.append(buffer, size_read);
	if (client.action != ACTION_CGI)
	{
		if (size_read == _event_list[i].data)
		{
			close(client.fd_read);
			client.response.read_end = true;
			client.state = READY;
			process_client(client.fd);
			return ;
		}
	}
	else if (_event_list[i].flags & EV_EOF)
	{
		close(client.fd_read);
		waitpid(client.cgi.pid, &client.cgi.exit_code, WNOHANG);	
		client.response.read_end = true;
		client.state = READY;
		process_client(client.fd);
		return ;
	}
	if (size_read == NB_READ)
	{
		client.state = READY;
		process_client(client.fd);
	}
}

// upload 
void Worker::write_client(int i)
{
#ifdef DEBUG
	std::cout << "[Worker] -   Write client : \n";
#endif 
	Socket_client & client = retrieve_client((long)_event_list[i].udata);
	ssize_t			size_write = client.request.body.size();
#ifdef DEBUG
	client.what();
	std::cout << ", " << _event_list[i].data << " bytes to write\n";
#endif 

	if (_event_list[i].flags & EV_ERROR)
	{
		close(client.fd_write);
		close(client.fd_read);
		client.action = ACTION_NORMAL;
		client._update_stat(RESPONSE | ERROR, 502);
		process_client(client.fd);
		std::cout << "EV_ERROR\n";
		return ;
	}
	if (client.action == ACTION_CGI)
	{
		if (waitpid(client.cgi.pid, &client.cgi.exit_code, WNOHANG) == client.cgi.pid)
		{
			close(client.fd_write);
			close(client.fd_read);
			client.action = ACTION_NORMAL;
			client._update_stat(RESPONSE | ERROR, 502);
			process_client(client.fd);
			std::cout << "WAITPID\n";
			return ;
		}
		size_write = std::min(client.request.body.size(), (size_t)_event_list[i].data);
	}
	size_write = write(client.fd_write, client.request.body.c_str(), size_write);
	if (size_write == -1) {
		close(client.fd_write);
		if (client.action != ACTION_CGI) {
			client._update_stat(RESPONSE | ERROR, 500);
			process_client(client.fd);
		}
		return;
	}
	if ((size_t)size_write < client.request.body.size())
	{
		client.request.body.erase(0, size_write);
	}
	else 
	{
		client.state &= ~NEED_WRITE;
		client.request.body.clear();
		close(client.fd_write);
		if(client.action != ACTION_CGI) {
			client.state = READY;
			client.response.read_end = true;
			process_client(client.fd);
		}
	}
}

void Worker::process_client(int fd_client)
{
	#ifdef DEBUG
		std::cout << "[Worker] -   process client : \n";
	#endif 

	Socket_client & client = retrieve_client(fd_client);

	#ifdef DEBUG
		client.what();
		std::cout << "\n";
	#endif 
	if (!(client.state & RESPONSE) && !(client.state & READY)) 
	{
		if (client.state & REQUEST_LINE)
			client.process_request_line();
		if (client.state & HEADERS)
			client.process_headers();
		if (client.state & ROUTE)
		    client.prepare_response();
		if (client.state & BODY) 
		{
			update_modif_list(client.fd, EVFILT_TIMER,
				EV_ADD | EV_ONESHOT, NOTE_SECONDS, TO_BODY);
			client.process_body();
		}
		if (client.state & RESPONSE) {
			update_modif_list(client.fd, EVFILT_TIMER,
					EV_ADD | EV_ONESHOT, NOTE_SECONDS, TO_RESPONSE);
			if (client.closed)
				update_modif_list(client.fd, EVFILT_READ, EV_DELETE);
		}
	}
	
	if (client.state & RESPONSE)
	{
		client.process_response();
		if (client.state & NEED_READ)
			update_modif_list(client.fd_read, EVFILT_READ , EV_ADD,
				0, 0,(void *)((long)client.fd));
		if (client.state & NEED_WRITE)
			update_modif_list(client.fd_write, EVFILT_WRITE, EV_ADD,
					0, 0,(void *)((long)client.fd));
	}
	if(client.state == READY) {
		client.fetch_response();
		update_modif_list(client.fd, EVFILT_WRITE, EV_ADD);
	}
}

static void handler(int num)
{
	(void)num;
	std::cerr << "SIGPIPE\n"; 
}

void Worker::event_loop(void)
{
	int					kq;
	std::map<int, Socket_server>::iterator last = _socket_servers.end();

	signal(SIGPIPE, handler);
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
#ifdef DEBUG
		std::cout << "-----\n" ;
#endif
		for (int i = 0; i < number_of_events; i++)
		{
			try 
			{ 
				/* nouvelle connexion client */
				if (_socket_servers.find(_event_list[i].ident) != last)
					new_client(i);
				else
				{
					if (_closed_clients.find(_event_list[i].ident) != _closed_clients.end())
					   continue;	
					else if(_event_list[i].udata && _socket_clients.find((long)_event_list[i].udata) == _socket_clients.end())
						continue ;
					else if(!(_event_list[i].udata) &&  _socket_clients.find(_event_list[i].ident) == _socket_clients.end())
						continue ;
					/* fin de connexion client */
					else if  (!(_event_list[i].udata) && 
							(_event_list[i].flags & EV_EOF || _event_list[i].filter == EVFILT_TIMER))
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
					}
				}
			}
			catch (std::exception & e)
			{
				std::cerr << "Worker::event_loop: " << e.what() << std::endl;
			}
		}
		/* We have at least one event registered per client and at most four
		 * for CGI cases, we setup event_list to receive at most three times
		 * socket_client size, the up average */
		_event_list.resize(20);
	}
}
