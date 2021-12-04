#include <iostream>

typedef enum http_state {
	REQUEST_LINE = 1,
	HEADERS = 2,
	ROUTE = 4,
	BODY = 8,
	RESPONSE = 16,
	SETUP_CGI = 32,
	NEED_READ = 64,
	NEED_WRITE = 128,
	READY = 256,
	WAIT_CGI = 512,
	READY_CGI = 1024,
	ERROR = 2048,
	CLOSED = 4096,
} e_http_state;

void what_state(int state)
{
	std::cout << "----- Socket_client state : -----" << std::endl;
	if (state & REQUEST_LINE)
		std::cout << "REQUEST_LINE" << std::endl;
	if (state & HEADERS)
		std::cout << "HEADERS" << std::endl;
	if (state & ROUTE)
		std::cout << "ROUTE" << std::endl;
	if (state & BODY)
		std::cout << "BODY" << std::endl;
	if (state & RESPONSE)
		std::cout << "RESPONSE" << std::endl;
	if (state & SETUP_CGI)
		std::cout << "SETUP_CGI" << std::endl;
	if (state & NEED_READ)
		std::cout << "NEED_READ" << std::endl;
	if (state & NEED_WRITE)
		std::cout << "NEED_WRITE" << std::endl;
	if (state & READY)
		std::cout << "READY" << std::endl;
	if (state & WAIT_CGI)
		std::cout << "WAIT_CGI" << std::endl;
	if (state & READY_CGI)
		std::cout << "READY_CGI" << std::endl;
	if (state & ERROR)
		std::cout << "ERROR" << std::endl;
	if (state & CLOSED)
		std::cout << "CLOSED" << std::endl;
}

int main()
{
	int state = 0;

	state |= READY;
	state |= ERROR;
	what_state(state);
	return 0;
}