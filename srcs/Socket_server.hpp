#include <iostream>
#include <arpa/inet.h>

class Socket_server {
	public:
		Socket_server(u_short port, uint32_t address);
		void listen();
		void bind();
	private:
		int fd;
		u_short port;
		uint32_t address;
};
