#include <vector>
#include <string>
#include <iostream>
#define SPACE 1
#define TRAIL 3

const std::string & check_method (std::string & buffer_recv)
{
	static const std::string	methods[] = {
		"GET", "POST", "PUT", "DELETE", "err"};
	int i;

	for (i = 0; i < 4; i++)
		if (!buffer_recv.compare(0, methods[i].size(),
								methods[i]))
			break;
	return (methods[i]);
}

bool process_request_line(std::string & buffer_recv)
{
	static const std::string	version = "HTTP/1.";
	size_t 						found;

	found = buffer_recv.find("\r\n");
	if (found == std::string::npos)
		return false;
	const std::string & method = check_method(buffer_recv);
	if (method == "err" || buffer_recv[method.size()] != ' ')
		return false;
	found = buffer_recv.find(" ", method.size() + SPACE);
	if (found == std::string::npos)
		return false;
	const std::string uri = buffer_recv.substr(method.size() + SPACE,
		   						found - (method.size() + SPACE));	
	found = buffer_recv.find(version,
								method.size() + uri.size() + SPACE * 2);
	if ((found == std::string::npos) ||
		/* doest minor version is a digit ? */ 
		!isdigit(buffer_recv[found + version.size()]) || 
		/* [METHOD SPACE URI SPACE HTTP/x.x\r\n */
		((method.size() + uri.size() + version.size() + 2 * SPACE + TRAIL) 
		!= buffer_recv.size()))
		return false;
	//std::cout << "URI is [" << uri << "]\n";
	return true;
	/* erase extracted string as late as possible */
}

void process_headers(std::string & buffer_recv)
{
	std::map<std::string, std::string> headers;

	found = buffer_recv.find("\r\n");
	if (found == std::string::npos)
		return false;
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
	ref.push_back(std::make_pair("GET\r\n", false));
	ref.push_back(std::make_pair(" GET \r\n", false));
	ref.push_back(std::make_pair("GET / GET \r\n", false));

	for (size_t i = 0; i < ref.size(); i++)
	{
		if (process_request_line(ref[i].first) != ref[i].second)
			std::cout << "[KO] - " << ref[i].first;
		else
			std::cout << "[OK] - " << ref[i].first;
	}
}
