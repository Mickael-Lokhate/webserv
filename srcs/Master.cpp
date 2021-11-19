#include "Master.hpp"

Master::Master(std::string const & file_conf) : _file_conf(file_conf), _servers(), _sockets_servers() { 
	std::cout << "Master()" << "\n";
}

void Master::init() {
	std::cout << "init()" << "\n";
	// si _file_conf est vide ou que le fichier n'existe pas
	// alors ouvrir le fichier par default
	// afficher le choix de la config sur stdin
	// envoyer le ifsteam aux loader
	std::ifstream ifs;
	Loader loader(ifs);
	try {
		loader.add_servers(_servers);
	} catch (std::exception const & e) {
		std::cerr << "Error : " << e.what() << "\n";
	}

	// recuperer les post:adress des servers
	// creer les sockets & bind
	
}

void Master::work() {
	std::cout << "work()" << "\n";
	//for_each(_sockets_servers.begin(), _sockets_servers.end(), _listen);
	//Worker worker(_servers, _sockets_servers);
	//Worker.event_loop();
}

void Master::what() {
	std::cout << "what()" << "\n";

	// affiche file_conf
	std::cout << "_DEFAULT_CONF: " << _DEFAULT_CONF << "\n";
	std::cout << "_file_conf: " << _file_conf << "\n";

	// affiche tt les serveur avec leurs routes
	//for_each(_servers.begin(), _servers.end(), _what);

	// affiche tt les sockets_serveurs
	//for_each(_servers.begin(), _servers.end(), _what);
}

void Master::_listen(Socket_server socket_server) {
	socket_server.listen();
}

template<class T>
void Master::_what(T elmt) {
	elmt.what();	
}
