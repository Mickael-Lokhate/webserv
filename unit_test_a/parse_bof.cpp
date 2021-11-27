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

	//std::cout << "Request line : " << request_line << "\n";
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
	//std::cout << "Method : [" << method << "]\n";
	//std::cout << "Uri : [" << uri << "]\n";
	//std::cout << "Protocol : [" << protocol << "]\n";
	/* erase extracted string as late as possible */
	/* END - request line */
}

int main(void)
{
	std::vector<std::pair<std::string, bool> >ref;

	ref.push_back(std::make_pair("GET / HTTP\r\n", false));
	ref.push_back(std::make_pair("GET / HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("GET /aurelien.html HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("PUT / HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("POST / HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("DELETE / HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("DDELETE / HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("/ HTTP\r\n", false));
	ref.push_back(std::make_pair("GET / \r\n", false));
	ref.push_back(std::make_pair("GETTT / \r\n", false));
	ref.push_back(std::make_pair("                      GET \r\n", false));
	ref.push_back(std::make_pair("\r\n", false));
	ref.push_back(std::make_pair("", false));
	ref.push_back(std::make_pair("GET  / HTTP/1.1 \r\n", false));
	ref.push_back(std::make_pair("GET /index.html HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("GET ////// HTTP/1.1\r\n", true));
	ref.push_back(std::make_pair("GET HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET/HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET/ HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET/ HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET / HTTP /1.1\r\n", false));
	ref.push_back(std::make_pair("GET / HTTP/ 1.1\r\n", false));
	ref.push_back(std::make_pair("GET / HTTP/1.1 \r\n", false));
	ref.push_back(std::make_pair("GET / GET HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET PUT / HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET / GET GET HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair("GET / HTTP/1.1 HTTP/1.1\r\n", false));
	ref.push_back(std::make_pair(" GET / HTTP/1.1\r\n", false));

	for (size_t i = 0; i < ref.size(); i++)
	{
		if (process_header(ref[i].first) != ref[i].second)
			std::cout << "[KO] - " << ref[i].first;
		else
			std::cout << "[OK] - " << ref[i].first;
	}
}
