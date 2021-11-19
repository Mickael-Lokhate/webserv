#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Socket_server.hpp"
#include "Server.hpp"
#include "Loader.hpp"
#include "Worker.hpp"

class Master {
	private:
		static const std::string _DEFAULT_CONF;
		std::string _file_conf;
		std::vector<Server> _servers;
		std::vector<Socket_server> _sockets_servers;

	public:
		Master(std::string const & file_config);
		void init();
		void work();
		void what();
	private:
		void _listen(Socket_server ss);
		template<class T>
		void _what(T elmt);
};
