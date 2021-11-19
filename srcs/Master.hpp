#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Server.hpp"

class Master {
	private:
		static const std::string _DEFAULT_CONF;
		std::string _file_conf;
		vector<Server> _servers;
		vector<Socket_server> _socket_servers;

	public:
		Master(std::string const & file_config);
		void init();
		void work();
		std:string const & to_string();
	private:
		void _listen(Socket_server ss) throw ;
		std::stringstream const & _print_server;

};
