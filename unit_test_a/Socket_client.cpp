#include <iostream>
#include "Socket_client.hpp"

Socket_client::Socket_client(void)
{
}

Socket_client::Socket_client(int fd, const std::string & addr, 
								const std::string & port) :
	fd(fd),
	addr(addr),
	port(port)
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
	addr = ref.addr;
	port = ref.port;
	return *this;
}

/* parser */
void Socket_client::build_request()
{

	//	1 - REQUEST_LINE - Getline(stream, "\r\n")
	//		+ extraire les champs, ajouter les attributs et vérifier
	//			1 - Méthode HTTP(GET, POST, PUT, DELETE)
	//			2 - URI(Réf. BNF rfc 7230)
	//				Cas spécifique : résoudre les . et .., décoder %XX.
	//			3 - Version HTTP (>= HTTP/1.1)

	get_request_line();
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
}

void Socket_client::what(void) const
{
	std::cout << "[" << fd << "] - " << addr << ":" << port;
}
