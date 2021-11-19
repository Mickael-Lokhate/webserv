#include "Master.hpp"

Master::Master(std::string const & file_conf) : _file_conf(file_conf),  { 
}

void Master::init() {
	// si _file_conf est vide ou que le fichier n'existe pas
	// alors ouvrir le fichier par default
	// afficher le choix de la config sur stdin
	// envoyer le ifsteam aux loader
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
	for_each(_socket_servers.begin(), _socket_servers.end(), _listen);
	Worker worker(_servers, _socket_servers);
	wroker.event_loop();
}

void Matser::what() {
	// affiche file_conf
	ss << "_DEFAULT_CONF: " << _DEFAULT_CONF << "\n";
	ss << "_file_conf: " << _file_conf << "\n";

	// affiche tt les serveur avec leurs routes
	for_each(_servers.begin(), _servers.end(), _print_server);
	
	return ss;
}
