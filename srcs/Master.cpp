#include "Master.hpp"

const std::string Master::_DEFAULT_CONF = "default.conf";

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
		throw std::runtime_error(strerror(errno));
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
		_add_servers(socket_server);
		socket_server.bind_();
		_socket_servers.insert(std::make_pair(socket_server.fd, socket_server));
	}
	
}

void Master::_add_servers(Socket_server & socket_server) {
	std::vector<Server>::iterator iter = _servers.begin();
	while (iter != _servers.end()) {
		if (iter->address == socket_server.address && iter->port == iter->port)
			socket_server.servers.push_back(&(*iter));
    	iter++;
	}
}


void Master::work() {
	for(std::map<int, Socket_server>::iterator it = _socket_servers.begin(); it !=_socket_servers.end(); it++)
		it->second.listen_();
	Worker worker(_socket_servers);
	worker.event_loop();
}

void Master::what() {

	// affiche file_conf
	std::cout << "_DEFAULT_CONF: " << _DEFAULT_CONF << "\n";
	std::cout << "_file_conf: " << _file_conf << "\n";

	for_each(_servers.begin(), _servers.end(), _what_vector<Server>);
	for(std::map<int, Socket_server>::iterator it = _socket_servers.begin(); it !=_socket_servers.end(); it++)
		it->second.what();

}
