#include <iostream>
#include <string>
#include "Socket_client.hpp"

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

bool Socket_client::check_method(std::string & in)
{
	static const std::string	methods[] = {
		"GET", "POST", "PUT", "DELETE"};

	for (int i = 0; i < 4; i++)
		if (!in.compare(methods[i]))
			return true;
	return false;
}


bool Socket_client::process_header(void)
{
	size_t 						found;

	/* START - request line */
	found = buffer_recv.find("\r\n");
	if (found == std::string::npos)
		return false;
	std::string		request_line = buffer_recv.substr(0, found);
	std::string		method;
	std::string		uri;
	std::string		protocol;

	std::cout << "Request line : " << request_line;
	short i, state = METHOD;

	for (i = 0; request_line[i]; i++)
	{
		if (request_line[i] == ' ')
		{
			switch (state)
			{
				case METHOD:
					method.append(request_line, i);
					if (!check_method(method))
						return false;
					break;
				case URI: 
					uri.append(request_line, method.size(),
								i - method.size());
					break;
			}
			state++;
		}
	}
	protocol.append(request_line, method.size() + uri.size(),
						i - (method.size() + uri.size()));
	std::cout << "Request line : [" << request_line << "]\n";
	std::cout << "Uri : [" << uri << "]\n";
	std::cout << "Protocol : " << protocol << "]\n";
	/* erase extracted string as late as possible */
	/* END - request line */
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
