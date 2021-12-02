#include "Socket_client.hpp"

Socket_client::Socket_client(int fd, const std::string & addr, 
								const std::string & port, Socket_server * socket_server) :
	fd(fd),
	addr(addr),
	port(port),
	state(REQUEST_LINE),
	socket_server(socket_server),
	fd_read(-1),
	fd_write(-1)
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
	cgi.what();
	std::cout << "fd_read : {" << fd_read << "}\n";
	std::cout << "fd_write : {" << fd_write << "}\n";
	socket_server->big_what();
}

bool Socket_client::is_valid_uri(std::string const & str) {
	if (str[0] != '/')
		return false;
	return true;
}

static void _abort(void)
{
	std::cout << "[Cgi] - child: " << std::string(strerror(errno)) << std::endl;
	_exit(EXIT_FAILURE);
}

static std::string _extract_query(const std::string & uri)
{
	size_t found;

	if ((found = uri.find("?")) == std::string::npos)
		return "";
	else return uri.substr(found + 1, std::string::npos);
}

static std::string _delete_query(const std::string & uri)
{
	size_t found;


	if ((found = uri.find("?")) == std::string::npos)
		return uri;
	else return uri.substr(0, found);
}

void Socket_client::setup_cgi()
{
	cgi.query_string = "QUERY_STRING=" + _extract_query(request.uri);
	cgi.request_method = "REQUEST_METHOD=" + request.method;
	cgi.content_type = "CONTENT_TYPE=" + request.headers["content-type"];
	cgi.content_length = "CONTENT_LENGTH=" + request.headers["content-length"];
	cgi.path_translated = "PATH_TRANSLATED=" + route.root.first + request.uri;
	cgi.script_name = "SCRIPT_NAME=" + _delete_query(request.uri);
	cgi.request_uri = "REQUEST_URI=" + request.uri;
	cgi.document_uri = "DOCUMENT_URI=" + _delete_query(request.uri);
	cgi.remote_addr = "REMOTE_ADDR=" + addr;
	cgi.remote_port = "REMOTE_PORT=" + port;
	cgi.server_port = "SERVER_PORT=" + server->port;
	cgi.server_name = "SERVER_NAME=" + server->address;
	cgi.script_filename = "SCRIPT_FILENAME=" + route.cgi;
	cgi.path_info = "PATH_INFO=" + _delete_query(request.uri);

	char *path = getenv("PATH");
	char *pwd = getenv("PWD");

	if (path)
		cgi.envp.push_back(path);
	if (pwd)
		cgi.envp.push_back(pwd);
	/* static environment */
	cgi.envp.push_back(cgi.server_protocol.begin().base());
	cgi.envp.push_back(cgi.request_scheme.begin().base());
	cgi.envp.push_back(cgi.gateway_interface.begin().base());
	cgi.envp.push_back(cgi.server_software.begin().base());
	/* ------------------ */
	cgi.envp.push_back(cgi.query_string.begin().base());
	cgi.envp.push_back(cgi.request_method.begin().base());
	cgi.envp.push_back(cgi.content_type.begin().base());
	cgi.envp.push_back(cgi.content_length.begin().base());
	cgi.envp.push_back(cgi.script_name.begin().base());
	cgi.envp.push_back(cgi.request_uri.begin().base());
	cgi.envp.push_back(cgi.document_uri.begin().base());
	cgi.envp.push_back(cgi.remote_addr.begin().base());
	cgi.envp.push_back(cgi.remote_port.begin().base());
	cgi.envp.push_back(cgi.server_name.begin().base());
	cgi.envp.push_back(cgi.script_filename.begin().base());
	cgi.envp.push_back(cgi.path_info.begin().base());
	cgi.envp.push_back(NULL);
}

/*
 
   		  Le Worker écrit dans INPUT[1] et CGI lit dans INPUT[0]
   		 Le Worker lit dans OUTPUT[0] et CGI écrit dans OUTPUT[1]
 
 
 			                  ----------------


 
               input[1] <=> write_fs                    input[0]

                +-+-------------------------------------+-+
      +------>  | |            |---------->             | |  +------+
      |         +-+-------------------------------------+-+         |
      +                                                             v
                         +----------------------+
+----------+             |  Cgi {               |             +----------+
|          |             |                      |             |          |
|  Worker  |             |      int input[2];   |             |    Cgi   |
|          |             |      int output[2];  |             |          |
+----------+             |  };                  |             +----------+
                         +----------------------+
      ^                                                             +
      |         +-+-------------------------------------+-+         |
      +------+  | |            <----------|             | |  <------+
                +-+-------------------------------------+-+

            output[0] <=> read_fs                    output[1]

*/

void Socket_client::prepare_pipes(void)
{

#ifdef DEBUG
	std::cout << "[Cgi] - start CGI initialization" << std::endl;
#endif
	if (pipe(cgi.input) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
	if (pipe(cgi.output) == -1)
	{
		std::runtime_error error =
				std::runtime_error(std::string(strerror(errno)));
		close(cgi.input[0]);
		close(cgi.input[1]);
		throw error;
	}
	fd_read = cgi.output[0];
	fd_write = cgi.input[1];
}

void Socket_client::process_cgi(void)
{
	pid_t pid;

#ifdef DEBUG
	std::cout << "[Cgi] - start CGI execution" << std::endl;
#endif
	prepare_pipes();
	if ((pid = fork()) == -1)
	{
		cgi.close_pipe_worker_side();
		throw std::runtime_error(std::string(strerror(errno)));
	}
	else if (pid == 0)
	{
		/* we SHOULD close socket_clients, socket_server and
		 * ALL currently opens fd..
		 */
		cgi.close_pipe_cgi_side();
		if (dup2(cgi.input[0], STDIN_FILENO)   == -1 ||
			dup2(cgi.output[1], STDOUT_FILENO) == -1)
			_abort();
		if (execve(route.cgi.c_str(), (char *[]){route.cgi.begin().base(),
			_delete_query(request.uri).begin().base(), NULL}, cgi.envp.begin().base()) == -1)
		/* Catch error with waitpid */
			_abort();
	}
	cgi.close_pipe_worker_side();
	cgi.pid = pid;
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
			server = *it;
			break;
		}
		it++;
	}
	if (it == socket_server->servers.end()) {
		route = (socket_server->servers[0])->choose_route("");
		server = socket_server->servers[0];
	}
	route.what();
	//	detecte error case relative to request 
	//	and update response.status
}

void Socket_client::process_response() {
	// return
	// upload
	// cgi
	// get folder or file ou error_file(when response.status == 400 501 405 ...)
}

void process_cgi() {
}
