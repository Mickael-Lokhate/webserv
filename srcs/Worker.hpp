#pragma once
#include <string>
#include <map>
#include <vector>
#include "Socket_server.hpp"
#include "Socket_client.hpp"
/*
		+---------------------------------------------------------------------+
		|                                                                     |
		|                              Worker                                 |
		|                                                                     |
		|                                                                     |
		|                           event_loop()                              |
		|                                                                     |
		|     +---------------------------+                                   |
		|     |                                                               |
		|     +--> new_client()+--------------------------------> accept()    |
		|     |                                                               |
		|     +--> recv_client()+-+                                           |
		|     |                   |                                           |
		|     +--> send_client()+------------------------+                    |
		|     |                   |                      |                    |
		|     +--> del_client()+--------------------------------> close()     |
		|                         |                      |                    |
		|                         |                      |                    |
		|                         |                      |                    |
		|                         v                      v                    |
		|                  build_request()       build_response()             |
		|                         +                      +                    |
		|                         |                      |                    |
		|    +--------------------+                      |                    |
		|    |                                           |                    |
		|    |                                           |                    |
		|  +---------------------------------------------+                    |
		|  | |          +---------------------------------------------------+ |
		|  | +----> req |GET / HTTP/1.1\r\nHost: example.com\r\nGET / HTTP/1| |
		|  |            +---------------------------------------------------+ |
		|  |                                                                  |
		|  |  * Aggrégation des données client                      ^         |
		|  |                                                        |         |
		|  |  * Vérification de la requête                          +         |
		|  |                                                      cursor      |
		|  |  * Création de l'object Request                                  |
		|  |                                                                  |
		|  |               ----------+ Gestion Client +----------             |
		|  |                                                                  |
		|  |  * Construction de la réponse(fichiers, CGI...)                  |
		|  |                                                                  |
		|  |  * Récupération des fichiers associés                            |
		|  |                                                                  |
		|  |  * Emission des données, fragmentée si nécessaire                |
		|  |                                                                  |
		|  |                                                                  |
		|  |            +---------------------------------------------------+ |
		|  +------> rep |HTTP 200 OK\r\nContent_type: blablabla...          | |
		|               +---------------------------------------------------+ |
		+---------------------------------------------------------------------++
*/

#include "Socket_client.hpp"
#include "Socket_server.hpp"
#include "Server.hpp"

class Worker {

	public:

	Worker(const std::vector<Server> & servers,
			const std::map<int, Socket_server> & socket_servers);
	Worker(const Worker & ref);
	~Worker(void);
	Worker & operator=(const Worker & right);

	void event_loop(void);
	void what(void) const;

	private:

	void new_client(int socket);
	void recv_client(const Socket_client & c);
	void send_client(const Socket_client & c);
	void del_client(const Socket_client & c);
	
	std::map<int, Socket_client>	_socket_clients;
	std::map<int, Socket_server>	_socket_servers;
	const std::vector<Server>		_servers;

};
