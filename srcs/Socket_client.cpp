#include "Socket_client.hpp"

Socket_client::Socket_client(int fd, const std::string & addr, 
								const std::string & port, Socket_server * socket_server) :
	fd(fd),
	addr(addr),
	port(port),
	state(REQUEST_LINE),
	socket_server(socket_server),
	fd_read(-1),
	fd_write(-1),
	pid_cgi(-1)
{
	;
}

Socket_client::Socket_client(const Socket_client & ref)
{
	*this = ref;
}

Socket_client::~Socket_client(void)
{
	;
}

Socket_client & Socket_client::operator=(const Socket_client & ref)
{
	fd = ref.fd;
	buffer_recv = ref.buffer_recv;
	buffer_send = ref.buffer_send;
	addr = ref.addr;
	port = ref.port;
	state = ref.state;
	request = Request();
	response = ref.response;
	route = ref.route;
	socket_server = ref.socket_server;
	fd_read = ref.fd_read;
	fd_write = ref.fd_write;
	pid_cgi = ref.pid_cgi;
	return *this;
}

void Socket_client::what(void) const
{
	std::cout << "[" << fd << "] - " << addr << ":" << port;
}

void Socket_client::big_what(void) const
{
	std::cout << "fd : {" << fd << "}\n";
	std::cout << "addr : {" << addr << "}\n";
   	std::cout << "port : {" << port << "}\n";
	std::cout << "buffer_recv : {" << buffer_recv << "}\n";
	std::cout << "buffer_send : {" << buffer_send << "}\n";
	std::cout << "state : {" << state << "}\n";
	request.what();
	response.what();
	route.what();
	std::cout << "fd_read : {" << fd_read << "}\n";
	std::cout << "fd_write : {" << fd_write << "}\n";
	std::cout << "pid_cgi : {" << pid_cgi << "}\n";
	socket_server->big_what();
}

bool Socket_client::is_valid_uri(std::string const & str) {
	if (str[0] != '/')
		return false;
	return true;
}

const std::string & Socket_client::check_method() {
	static const std::string	methods[] = {
		"GET", "POST", "PUT", "DELETE", "err"};
	int i;

	for (i = 0; i < 4; i++)
		if (!buffer_recv.compare(0, methods[i].size(),
								methods[i]))
			break;
	return (methods[i]);
}

void Socket_client::process_request_line()
{
	static const std::string	version = "HTTP/1.";
	size_t 						found;

	found = buffer_recv.find("\n");
	if (found == std::string::npos) {
		state = REQUEST_LINE;
		return;
	}
	if (!found || buffer_recv[found - 1] != '\r')
		request.delim = "\n";
	// empty line before request line
	if (buffer_recv.compare(0, request.delim.size(), request.delim) == 0) {
		buffer_recv.erase(0, request.delim.size());
		state = REQUEST_LINE;
		return;
	}
	request.method = check_method();
	if (request.method == "err" || buffer_recv[request.method.size()] != ' ')
	{
		response.status = 400;
		state = ROUTE;
		return;
	}
	found = buffer_recv.find(" ", request.method.size() + SPACE);
	if (found == std::string::npos) {
		response.status = 400;
		state = ROUTE;
		return;
	}
	request.uri = buffer_recv.substr(request.method.size() + SPACE,
		   					found - (request.method.size() + SPACE));
	if (!is_valid_uri(request.uri)) {
		response.status = 400;
		state = ROUTE;
		return ;
	}
	found = buffer_recv.find(version, request.method.size() + request.uri.size() +
							SPACE * 2);
	if ((found == std::string::npos) ||
		/* doest minor version is a digit ? */
		!isdigit(buffer_recv[found + version.size()]) ||
		buffer_recv.compare(found + version.size() + SPACE, 
		request.delim.size(), request.delim)) {
		response.status = 400;
		state = ROUTE;
		return;
	}
	state = HEADERS;
}

void Socket_client::process_headers()
{
	std::string							key;
	std::string							val;
	size_t								cursor = 0;
	size_t								found;
	size_t								colon;

	for (;;)
	{
		found = buffer_recv.find(request.delim, cursor);
		if (found == std::string::npos) {
			state = HEADERS;
			return;
		}
		if (found == cursor)
		{
			if (!request.host.empty())
			{
				buffer_recv.erase(0, found + request.delim.size());
				if (request.content_length == -1)
					request.content_length = 0;
				state = ROUTE;
				return;
			}
			else
			{
				response.status = 400;
				state = ROUTE;
				return;
			}
		}
		colon = buffer_recv.find(":", cursor);
		/* ignore invalid headers */
		if (colon == std::string::npos || colon > found)
		{
			cursor = found + request.delim.size();
			continue ;
		}
		key = buffer_recv.substr(cursor, colon - cursor);
		key = _tolower(key);
		val = buffer_recv.substr(colon + COLON,
				found - (colon + COLON));
		val = _tolower(_ltrim(_rtrim(val)));
		if (key == "host")
		{
			/* Host duplicate */
			if (!request.host.empty()) {
				response.status = 400;
				state = ROUTE;
				return;
			}
			request.host = val;
		}
		else if (key == "content-length")
		{
			/* Content-length duplicate */
			if (request.content_length != -1) {
				response.status = 400;
				state = ROUTE;
				return;
			}
			try {request.content_length = std::stoi(val); }
			catch (std::exception & e)
			{
				response.status = 400;
				state = ROUTE;
				return;
			}
		}
		else if (key == "transfer-encoding")
		{
			/* Transfer-encoding duplicate */
			if (request.headers.find(key) != request.headers.end())
			{
				response.status = 400;
				state = ROUTE;
				return;
			}
			if (val == "chunked")
				request.chunked = true;
			else 
			{
				response.status = 400;
				state = ROUTE;
				return;
			}
		}
		else
			request.headers[key] = val;
		cursor = found + request.delim.size();
		buffer_recv.erase(0, found + request.delim.size());
	}
}


bool Socket_client::get_simple_body() {
	size_t size = request.content_length - request.body.size();
	request.body.append(buffer_recv.substr(0, size));
	size = std::min(buffer_recv.size(), size);
	buffer_recv.erase(0, size);
	return (request.body.size() == (size_t)(request.content_length));
}

bool Socket_client::get_ckunked_body() {
	size_t length = 0;
	size_t pos_delim = 0;
	size_t cursor = 0;
	ssize_t size_chunck = -1;
	while (size_chunck != 0) {
		if ((pos_delim = buffer_recv.find(request.delim, cursor)) == std::string::npos) 
			break;
		// taille
		size_chunck = _hexstr_to_int(buffer_recv.substr(cursor, pos_delim));
		if (size_chunck < 0)
			throw std::logic_error("error chunked : hexa size");
		if (pos_delim + request.delim.size() + size_chunck + request.delim.size() > buffer_recv.size())
			break;
		#ifdef DEBUG1
		std::cout << "-> " << size_chunck << "\n";
		#endif
		cursor = pos_delim + request.delim.size();
		request.body.append(buffer_recv.substr(cursor, size_chunck));
		#ifdef DEBUG1
		std::cout << " {" <<  buffer_recv.substr(cursor, size_chunck) << "}\n";
		#endif
		cursor += size_chunck;
		if (buffer_recv.compare(cursor, request.delim.size(), request.delim) != 0)
			throw std::logic_error("error chunked : delimiter expected");
		cursor += request.delim.size();
		length += size_chunck;
	}
	buffer_recv.erase(0, cursor);
	request.content_length += length;
	return (size_chunck == 0);
}


void Socket_client::process_body() {
	if (!(request.method == "POST" || request.method == "PUT")) {
		state = RESPONSE;
		return;
	}
	if (!request.chunked) {
		if (get_simple_body()) {
			state = RESPONSE;
			return;
		}
	} else {
		try {
			if (get_ckunked_body()) {
				state =  RESPONSE;
				return ;
			}
		} catch(std::logic_error const & e) {
			#ifdef DEBUG
			std::cout << "body :  {" << request.body << "}\n";
			std::cout << "content-length :  {" << request.content_length << "}\n";
			std::cout << "rest : {" << buffer_recv << "}\n" ;
			std::cout << e.what() << "\n";
			#endif
			response.status = 400;
			state = RESPONSE;
			return ;
		}
	}
	state = BODY;
}


void Socket_client::prepare_response() {
	//	choose server + route
	//big_what();
	std::vector<Server *>::iterator it = socket_server->servers.begin();
	while (it != socket_server->servers.end()) {
		if (find((*it)->server_name.begin(), (*it)->server_name.end(), request.host) != (*it)->server_name.end()) {
			route = (*it)->choose_route(request.uri);
			break;
		}
		it++;
	}
	if (it == socket_server->servers.end())
		route = (socket_server->servers[0])->choose_route("");
	route.what();
	//	detecte error case relative to request 
	//	and update response.status
}

void Socket_client::process_response() {
	// Check if response status is != 0
	if (response.status != 0)
		_process_error_page();
	// return
	else if (!route.return_.first.empty())
		_process_return();
	// upload
	else if (!route.upload.empty())
		_process_upload();
	else if (!route.cgi.empty())
		process_cgi();
	else
		_process_normal();
	// cgi
	// get folder or file ou error_file(when response.status == 400 501 405 ...)
}

void Socket_client::_process_error_page()
{
	// state = NEED_READ;
	// check if response.status corresponding to an error_page
	// Open response.status corresponding file
	// update response.body
	// if bad file in an error page => 404 
}

void Socket_client::_process_return()
{
	response.status = to_number<short>(route.return_.first);
	response.location = route.return_.second;
	if ((fd_read = open(to_string(route.error_page.at(to_string(response.status))).c_str(), O_RDONLY | O_NONBLOCK)) == -1)
	{
		//TMP
		response.body = to_string(response.status);
	}
	state = NEED_READ;
	// state = READY;
}

void Socket_client::_process_upload()
{
	// Get Request body
	// state = NEED_WRITE;
	// Write uri file in the route.upload path
	// response.status = ?
	// state = READY;
}

void Socket_client::_process_normal()
{

	// Check method 
	// if get 
		// state = NEED_READ;
		// Try open requested file
		// update response.status depending on file opening
		// update response.body
		// state = READY;
	// if delete
		// try delete requested file
		// update response.status
		// state = READY
	// if post 
		// ?
	// if put
		// ?
	// if head
		// same as get but no body
}

void process_cgi() {
}
