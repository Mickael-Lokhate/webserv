#include <iostream>
#include <unistd.h>
#include <string>
#include <exception>
#include <sstream>
#include <map>

#define BUFFSIZE 1024

typedef enum http_state { 
	HEADERS,
	BODY,
	RESPONSE,
	ERROR,
} e_http_state;

std:: string buffer_recv;
e_http_state state = BODY;
std::string body;
std::map<std::string, std::string>	headers;

bool _chunked;
size_t _content_length;

ssize_t _hexstr_to_int(std::string hexstr) {
	size_t s;   
	if (!isxdigit(hexstr[0]))
		return -1;
	std::stringstream ss;
	ss << std::hex << hexstr;
	ss >> s;
	return s;
}

bool get_simple_body() {
	body.append(buffer_recv.substr(0, _content_length));
	return body.size() == _content_length;
}

bool get_ckunked_body() {
	size_t length = 0;
	std::string const DELIM = "\n";
	size_t pos_delim = 0;
	size_t cursor = 0;
	ssize_t size_chunck = -1;
	while (size_chunck != 0) {
		if ((pos_delim = buffer_recv.find(DELIM, cursor)) == std::string::npos) 
			break;
		size_chunck = _hexstr_to_int(buffer_recv.substr(cursor, pos_delim));
		if (size_chunck < 0)
			throw std::logic_error("error chunked : hexa size");
		if (pos_delim + DELIM.size() + size_chunck + DELIM.size() > buffer_recv.size())
			break;
		#ifdef DEBUG1
		std::cout << "-> " << size_chunck << "\n";
		#endif
		cursor = pos_delim + DELIM.size();
		body.append(buffer_recv.substr(cursor, size_chunck));
		#ifdef DEBUG1
		std::cout << " {" <<  buffer_recv.substr(cursor, size_chunck) << "}\n";
		#endif
		cursor += size_chunck;
		if (buffer_recv.compare(cursor, DELIM.size(), DELIM) != 0)
			throw std::logic_error("error chunked : delimiter expected");
		cursor += DELIM.size();
		length += size_chunck;
	}
	buffer_recv.erase(0, cursor);
	_content_length += length;
	return (size_chunck == 0);
}

std::string  _statetostr(e_http_state st) {
	switch (st) {
		case HEADERS:
			return "HEADERS";
		case BODY:
			return "BODY";
		case RESPONSE:
			return "RESPONSE";
		default:
			return "ERROR";
	}
}

int main(void) {
	char buff[BUFFSIZE + 1];
	int nb_read;
	_chunked = true;
	_content_length = 0;
	if (!_chunked)
		_content_length = 11;
	while(state == BODY) {
		while (buffer_recv.find(".") == std::string::npos) {
			nb_read = read(0, buff, BUFFSIZE);
			buffer_recv.append(buff, nb_read);
		}
		buffer_recv.erase(buffer_recv.size() - 2, 2);
		std::cout << "\n";
		if (!_chunked) {
			if (get_simple_body())
				state = RESPONSE;
		} else {
			try {
				if (get_ckunked_body())
					state = RESPONSE;
			} catch(std::logic_error const & e) {
				state = ERROR;
				#ifdef DEBUG
				std::cout << e.what() << "\n";
				#endif
			}
		}
		#ifdef DEBUG
		std::cout << "body :  {" << body << "}\n";
		std::cout << "content-length :  {" << _content_length << "}\n";
		std::cout << "rest : {" << buffer_recv << "}\n" ;
		std::cout << "state: " << _statetostr(state) << "\n";
		#endif
	}
}

/*
25
Voici les donnees du premier morceau

1B
et voici un second morceau

20
et voici deux derniers morceaux 
12
sans saut de ligne
0

*/
