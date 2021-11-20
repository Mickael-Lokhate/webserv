#include "Socket_server.hpp"
#include <unistd.h>

int main(int ac, char ** av) {
	try {
		Socket_server ss(av[1], av[2]);
		ss.what();
		ss.bind_();
		ss.listen_();
		read(0, (char []){0}, 1);
	} catch(std::string const & str) {
		std::cout << str << "\n";
	}
}
