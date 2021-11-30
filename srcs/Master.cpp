#include "Master.hpp"

static const std::string _DEFAULT_CONF = "default.conf";

Master::Master(std::string const & file_conf) : _file_conf(file_conf), _servers(), _socket_servers() { 
	std::cout << "Master()" << "\n";
}

void Master::init() {
	std::cout << "init()" << "\n";
	
	// choose file
	if (_file_conf.empty())
		_file_conf = _DEFAULT_CONF;
	std::ifstream ifs (_file_conf);
	if(!ifs.good())
		throw std::runtime_error(STRDEBUG0);
	Loader loader(ifs);
	try {
		loader.add_servers(_servers);
	} catch (std::exception const & e) {
		std::cerr << "Error : " << e.what() << "\n";
	}

	//create socket_srevers

	std::set<std::pair<std::string, std::string> > listens;
	for (std::vector<Server>::const_iterator it = _servers.begin(); it != _servers.end(); ++it)
			listens.insert(std::make_pair(it->address, it->port));

	for (std::set<std::pair<std::string, std::string> >::const_iterator it = listens.begin(); it != listens.end(); ++it) {
		Socket_server socket_server(it->first, it->second);
		socket_server.bind_();
		_socket_servers.insert(std::make_pair(socket_server.fd, socket_server));
	}

}

void Master::work() {
	for_each(_socket_servers.begin(), _socket_servers.end(), listen_);
	Worker worker(_servers, _socket_servers);
	worker.event_loop();
}

void Master::what() {

	// affiche file_conf
	std::cout << "_DEFAULT_CONF: " << _DEFAULT_CONF << "\n";
	std::cout << "_file_conf: " << _file_conf << "\n";

	// affiche tt les serveur avec leurs routes
	for_each(_servers.begin(), _servers.end(), what_);

	// affiche tt les sockets_serveurs
	for_each(_servers.begin(), _servers.end(), what_);
}

void Master::listen_(std::pair<int, Socket_server> & ss) {
	ss.second.listen_();
}
