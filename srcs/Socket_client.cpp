#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "Socket_client.hpp"
#define SPACE 1
#define TRAIL 3
#define COLON 1
#define DELIM 2

enum {
	METHOD,
	URI,
	PROTOCOL,
};

Socket_client::Socket_client(void)
{
}

Socket_client::Socket_client(int fd, const std::string & addr, 
								const std::string & port) :
	fd(fd),
	addr(addr),
	port(port),
	state(HEADERS)
{
}

Socket_client::Socket_client(const Socket_client & ref)
{
	*this = ref;
}

Socket_client::~Socket_client(void)
{
}

Socket_client & Socket_client::operator=(const Socket_client & ref)
{
	fd = ref.fd;
	//request = ref.request;
	buffer_recv = ref.buffer_recv;
	buffer_send = ref.buffer_send;
	addr = ref.addr;
	port = ref.port;
	state = ref.state;
	return *this;
}

static const std::string & check_method
(std::string & buffer_recv)
{
	static const std::string	methods[] = {
		"GET", "POST", "PUT", "DELETE", "err"};
	int i;

	for (i = 0; i < 4; i++)
		if (!buffer_recv.compare(0, methods[i].size(),
								methods[i]))
			break;
	return (methods[i]);
}

bool Socket_client::process_request_line(void)
{
	static const std::string	version = "HTTP/1.";
	size_t 						found;

	found = buffer_recv.find("\r\n");
	if (found == std::string::npos)
		return false;
	request.method = check_method(buffer_recv);
	if (method == "err" || buffer_recv[method.size()] != ' ')
		return false;
	found = buffer_recv.find(" ", method.size() + SPACE);
	if (found == std::string::npos)
		return false;
	request.uri = buffer_recv.substr(method.size() + SPACE,
		   					found - (method.size() + SPACE));
	found = buffer_recv.find(version, method.size() + uri.size() +
							SPACE * 2);
	if ((found == std::string::npos) ||
		/* doest minor version is a digit ? */
		!isdigit(buffer_recv[found + version.size()]) ||
		/* [METHOD SPACE URI SPACE HTTP/x.x\r\n */
		((method.size() + uri.size() + version.size() +
		2 * SPACE + TRAIL) != buffer_recv.size()))
		return false;
#ifdef DEBUG
	std::cout << "URI is [" << uri << "]\n";
#endif
	return true;
}

bool Socket_client::process_headers(void)
{
	std::string							key;
	std::string							val;
	size_t								cursor = 0;
	size_t								found;
	size_t								delim;

	for (;;)
	{
		found = buffer_recv.find("\r\n", cursor);
		if (found == std::string::npos || found == cursor)
			break ;
		delim = buffer_recv.find(":", cursor);
		/* ignore invalid headers */
		if (delim == std::string::npos || delim > found)
		{
			cursor = found + DELIM;
			continue ;
		}
		key = buffer_recv.substr(cursor, delim - cursor);
		val = buffer_recv.substr(delim + COLON, found - (delim + COLON));
		if (key == "Host")
		{
			/* Host duplicate */
			if (!request.host.empty())
				return false;
			request.host = val;
		}
		else if (key == "Content-length")
		{
			/* Content-length duplicate */
			if (!request.content_length.empty())
				return false;
			request.content_length = val;
		}
		else
			request.headers[key] = val;
		cursor = found + DELIM;
	}
#ifdef DEBUG
	for (std::map<std::string, std::string>::iterator it = request.headers.begin();
		it != request.headers.end(); it++)
		std::cout << it->first << ":" << it->second << std::endl;
#endif
	if (!host.empty())
		return true;
	else
		return false;
}

bool process_body(void)
{
}

/* parser */
bool Socket_client::build_request()
{
	return true;
		/*
		if (state == HEADER)
			process_header();
		if (state == BODY)
			process_body();
		*/

	//	1 - REQUEST_LINE - Getline(stream, "\r\n")
	//		+ extraire les champs, ajouter les attributs et vérifier
	//			1 - Méthode HTTP(GET, POST, PUT, DELETE)
	//			2 - URI(Réf. BNF rfc 7230)
	//				Cas spécifique : résoudre les . et .., décoder %XX.
	//			3 - Version HTTP (>= HTTP/1.1)

	//get_request_line();
	//	2 - HEADERS_FIELDS - Getline(stream, "\r\n")
	//		+ extraire les champs, remplir dans une map
	//			1 - Header name
	//			2 - Header value
	//			- lorsque la ligne est vide(\r\n\), vérifier les champs :
	//				- Si les Header Host|Content-length|Transfer-encoding sont dupliqué, renvoyer 400 Bad Request
	//				- Il faut au minimum le champ Host pour valider une requête, et aucun espace entre FIELD:VALUE
	//				- Si le Header est préfixé par X-, transformer tous les - en _ [CGI] et ajouter HTTP_ en préfix
	

	//	3 - BODY - Getline(stream, "\r\n")
	//		+ extraire la size Getline(stream, "\r\n"), en héxadécimal
	//		+ extraire la data Getline(stream, Chunk_size), qui doit être suivit de \r\n
	//			- Cas read_buffer_size < Chunk_size doit conserver l'état
	//			- Lorsque Chunk_size == 0, fin chunked
	//	
}

/* builder */
void Socket_client::build_response(void)
{
	//return open("test.txt", O_RDONLY);
}

void Socket_client::what(void) const
{
	std::cout << "[" << fd << "] - " << addr << ":" << port;
}
