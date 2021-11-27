#include <vector>
#include <string>
#include <iostream>

enum {
	METHOD,
	URI,
	PROTOCOL,
};

bool is_valid_uri(std::string & in)
{
	return true;
}

bool is_valid_method(std::string & in)
{
	static const std::string	methods[] = {
		"GET", "POST", "PUT", "DELETE"};

	for (int i = 0; i < 4; i++)
		if (!in.compare(methods[i]))
			return true;
	return false;
}

bool process_header(std::string & buffer_recv)
{
	size_t 						found;

	/* START - request line */
	found = buffer_recv.find("\r\n");
	if (found == std::string::npos)
		return false;
	std::string		request_line = buffer_recv.substr(0, found);
	std::string		method;
	std::string		uri;
	std::string		protocol;

	std::cout << "Request line : " << request_line << "\n";
	short i, state = METHOD;

	for (i = 0; request_line[i]; i++)
	{
		if (request_line[i] == ' ')
		{
			switch (state)
			{
				case METHOD:
					method.append(request_line, 0, i);
					if (!is_valid_method(method)) { return false; }
					break;
				case URI:
					uri.append(request_line, method.size() + state,
						i - method.size() - state); 
					if (!is_valid_uri(uri)) { return false; }
					break;
				default: return false;
			}
			state++;
		}
	}
	if (state == PROTOCOL)
		protocol.append(request_line, method.size() + uri.size() + state,
							i - (method.size() + uri.size()) - state);
	else 
		return false;
	std::cout << "Method : [" << method << "]\n";
	std::cout << "Uri : [" << uri << "]\n";
	std::cout << "Protocol : [" << protocol << "]\n";
	/* erase extracted string as late as possible */
	/* END - request line */
}

int main(void)
{
	std::vector<std::string> ref;

	ref.push_back("GET / HTTP\r\n");
	ref.push_back("GET / HTTP/1.1\r\n");
	ref.push_back("/ HTTP\r\n");
	ref.push_back("GET / \r\n");
	ref.push_back("                      GET \r\n");
	ref.push_back("\r\n");
	ref.push_back("");
	ref.push_back("GET  / HTTP/1.1 \r\n");
	ref.push_back("GET /index.html HTTP/1.1\r\n");

	for (int i = 0; i < ref.size(); i++)
	{
		process_header(ref[i]);
		std::cout << "--------" << std::endl;
	}
}
