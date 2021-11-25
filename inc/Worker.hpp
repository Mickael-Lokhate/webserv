/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aclerac <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 15:45:49 by aclerac           #+#    #+#             */
/*   Updated: 2021/11/24 19:39:50 by aclerac          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <set>
#include <map>
#include <vector>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include "Socket_server.hpp"
#include "Socket_client.hpp"
#define MAX_EVENTS 1024

class Worker {

	public:

	Worker(const std::map<int, Socket_server> & socket_servers);
	Worker(const Worker & ref);
	~Worker(void);
	Worker & operator=(const Worker & right);

	void event_loop(void);
	void what(void) const;

	private:

	void	register_socket_servers(void);
	void	update_modif_list(int fd, int16_t filter,
				uint16_t flags = 0, uint32_t fflags = 0,
				intptr_t data = 0);
	void	new_client(int i);
	void	recv_client(int i);
	void	send_client(int i);
	void	del_client(int i);
	
	std::vector<struct kevent>		_event_list;
	std::vector<struct kevent>		_modif_list;
	std::map<int, Socket_client>	_socket_clients;
	std::map<int, Socket_server>	_socket_servers;
	std::set<int>					_closed_clients;

};

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


