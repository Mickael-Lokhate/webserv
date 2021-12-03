#include "Socket_client.hpp"

extern std::map<short, std::string> status_msgs;
extern std::map<short, std::string> default_pages;

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

void Socket_client::generate_directory_listing(void)
{
	std::string					path = request.uri;
	DIR *						d = opendir(path.c_str());
	struct dirent * 			dp;
	struct stat					inf;
	char						buf[100];
	static const	std::string	top = "<!DOCTYPE html>\r\n<html>\r\n\t<head>\r\n\t<title>";
	static const	std::string bot = "\t</table>\r\n\t</body>\r\n</html>";
	static const	std::string col = "<tr>\r\n<th scope=\"col\">Name</th>\r\n<th scope=\"col\">"
		"Last-modified</th>\r\n<th scope=\"col\">Size</th>\r\n</tr>\r\n";
	static const	std::string sep = "\t\t<tr><th colspan=\"5\"><hr></th></tr>\r\n";

	if (!d)
		throw std::runtime_error(strerror(errno));
	response.body.append(top);
	response.body.append("Index of " + path + "</title>\r\n\t</head>\r\n\t<body>\r\n\t" +
				"<h1>Index of " + path + "</h1>\r\n\t<table>\r\n");
	response.body.append(col);
	response.body.append(sep);
	while ((dp = readdir(d)) != NULL)
	{
		if (!strftime(buf, 100, "%D %T", gmtime(&inf.st_mtimespec.tv_sec)))
			throw std::runtime_error("strftime");
		std::string file = path + "/" + dp->d_name;
		if (stat(file.c_str(), &inf) == -1)
			throw std::runtime_error(strerror(errno));
		response.body.append("\t\t<tr><td valign=\"top\"><a href=\"" + file + "\">" +
					/* Name */
					std::string(dp->d_name) + "</a></td>" +
					/* Last-modified */
					"<td align=\"right\">" + buf + "</td>" +
					/* Size */
					"<td align=\"right\">" + std::to_string(inf.st_size) +
					"</td></tr>\r\n");
	}
	response.body.append(sep);
	response.body.append(bot);
	closedir(d);
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

void Socket_client::_setup_cgi()
{
	cgi.path = "PATH=" + (getenv("PATH") ? std::string(getenv("PATH")) : "");
	cgi.pwd = "PWD=" + (getenv("PWD") ? std::string(getenv("PWD")) : "");
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

	cgi.envp.push_back(cgi.path.begin().base());
	cgi.envp.push_back(cgi.pwd.begin().base());
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

void Socket_client::_prepare_pipes(void)
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

void Socket_client::_exec_cgi(void)
{
	pid_t pid;

#ifdef DEBUG
	std::cout << "[Cgi] - start CGI execution" << std::endl;
#endif
	_prepare_pipes();
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

void Socket_client::_update_stat(int _state, short _status)
{
	state = _state;
	response.status = _status;
}

/* request-line   = method SP request-target SP HTTP-version CRLF */
void Socket_client::process_request_line()
{
	static const std::string	version = "HTTP/1.";
	size_t 						found;
	size_t						spaces = 0;

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
		_update_stat(ROUTE | ERROR | CLOSED, 400);
		return;
	}
	while (buffer_recv[request.method.size() + spaces] == ' ' )
		++spaces;
	found = buffer_recv.find(" ", request.method.size() + spaces);
	if (found == std::string::npos) {
		_update_stat(ROUTE | ERROR | CLOSED, 400);
		return ;
	}
	request.uri = buffer_recv.substr(request.method.size() + spaces,
		   					found - (request.method.size() + spaces));
	if (!is_valid_uri(request.uri)) {
		_update_stat(ROUTE | ERROR | CLOSED, 400);
		return ;
	}
	while (buffer_recv[request.method.size() +
			request.uri.size() + spaces] == ' ' )
		++spaces;
	found = buffer_recv.find(version, request.method.size() + request.uri.size() +
							spaces);
	if ((found == std::string::npos) ||
		/* doest minor version is a digit ? */
		!isdigit(buffer_recv[found + version.size()]) ||
		buffer_recv.compare(found + version.size() + DIGIT,
		request.delim.size(), request.delim)) 
	{
		_update_stat(ROUTE | ERROR | CLOSED, 400);
		return;
	}
	buffer_recv.erase(0, buffer_recv.find(request.delim) + request.delim.size());
	state = HEADERS;
}

void Socket_client::process_headers()
{
	std::string							key;
	std::string							val;
	size_t								cursor;
	size_t								found;
	size_t								colon;

	for (;;)
	{
		cursor = 0;
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
				_update_stat(ROUTE | ERROR | CLOSED, 400);
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
				_update_stat(ROUTE | ERROR | CLOSED, 400);
				return;
			}
			request.host = val;
		}
		else if (key == "content-length")
		{
			/* Content-length duplicate */
			if (request.content_length != -1) {
				_update_stat(ROUTE | ERROR | CLOSED, 400);
				return;
			}
			try {request.content_length = std::stoi(val); }
			catch (std::exception & e)
			{
				_update_stat(ROUTE | ERROR | CLOSED, 400);
				return;
			}
		}
		else if (key == "transfer-encoding")
		{
			/* Transfer-encoding duplicate */
			if (request.headers.find(key) != request.headers.end())
			{
				_update_stat(ROUTE | ERROR | CLOSED, 400);
				return;
			}
			if (val == "chunked")
				request.chunked = true;
			else 
			{
				_update_stat(ROUTE | ERROR | CLOSED, 501);
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
			state |= (CLOSED | ERROR);
			return ;
		}
	}
	if (!response.status && request.content_length > std::stol(route.max_body_size)) {
		response.status = 413;
		state = RESPONSE;
		state |= (CLOSED | ERROR);
		return;
	}	
	state = BODY;
}


void Socket_client::prepare_response() {
	//	choose server + route
	//	detecte error case relative to request 
	//	and update response.status

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
	// server->what();
	
	if (response.status & CLOSED) {
		state = RESPONSE;
		return ;
	}

	// limit_except
	state = BODY;
}

void Socket_client::process_response() {

	if (state & ERROR)
	{
		if (route.error_page.find(to_string(response.status)) != route.error_page.end())
			route = server->choose_route(route.error_page[to_string(response.status)]);
		else
			response.body = default_pages[response.status];
	}
	if (route.limit_except.size() && find(route.limit_except.begin(),
		route.limit_except.end(), request.method) == route.limit_except.end())
	{
		state |= ERROR;
		response.status = 405;
	}
	else if (!route.return_.first.empty())
		_process_return();
	else if (!route.upload.empty() && (request.method.compare("POST") == 0 || request.method.compare("PUT") == 0))
		_process_upload();
	else if (!route.cgi.empty())
		_process_cgi();
	else
		_process_normal();
	std::cout << "--RESPONSE--" << std::endl;
	response.what();
}

void Socket_client::_process_cgi() {
	if (state & SETUP_CGI) {
		_setup_cgi();
		_exec_cgi();
		state &= ~SETUP_CGI;
		state |= NEED_WRITE;
		state |= NEED_READ;
	//	state |= WAIT_CGI;
		return;
	}
	if (state & READY_CGI) {
		close(fd_write);
		close(fd_read);
		waitpid(cgi.pid, NULL, 0);
		std::cout << "body is >> " << response.body << std::endl;
		buffer_send.append(response.body);
		state |= READY;
		return;
	}

}

void Socket_client::_process_return()
{
	response.status = to_number<short>(route.return_.first);
	response.location = route.return_.second;
	state |= READY;
}

void Socket_client::_process_upload()
{
	if (_is_dir(route.upload.c_str()))
		return _set_error(403);
	if ((fd_read = open(route.upload.c_str(), O_RDWR | O_CREAT | O_NONBLOCK)) != -1)
	{
		state = NEED_WRITE;
		if (!response.status)
			response.status = 201; //created
	}
	return _set_error(403);

}

void Socket_client::_process_normal()
{
	std::string path = request.uri;
	if (*(path.end() - 1) != '/')
		path.push_back('/');
	if (!route.root.first.empty())
	{
		if (route.root.second)
			path = route.root.first;
		else
			path.insert(0, route.root.first);
	}
	if (path.at(0) == '/')
		path.insert(0, ".");
	if (request.method.compare("GET") == 0 || request.method.compare("HEAD") == 0)
	{
		if (_is_dir(path.c_str()))
		{
			if (!route.index.empty())
			{
				for (std::vector<std::string>::iterator it = route.index.begin(); it != route.index.end(); ++it)
				{
					if (*(path.end() - 1) != '/' && (*it).at(0) != '/')
						path.push_back('/');
					std::string tmp_path = path + *it;
					if ((fd_read = open(tmp_path.c_str(), O_RDONLY | O_NONBLOCK)) != -1)
					{
						if (!response.status)
							response.status = 200;
						response.content_length = _get_file_size(fd_read);
						if (request.method.compare("GET") == 0)
							state |= NEED_READ;
						else
							state |= READY;
						return ;
					}
				}
				if (route.autoindex.compare("on") == 0)
					generate_directory_listing();
				return _set_error(404);
			}
			if (route.autoindex.compare("on") == 0)
				generate_directory_listing();
			return _set_error(403);
		}
		else
		{
			if ((fd_read = open(path.c_str(), O_RDONLY | O_NONBLOCK)) != -1)
			{
				if (!response.status)
					response.status = 200;
				response.content_length = _get_file_size(fd_read);
				if (request.method.compare("GET") == 0)
					state |= NEED_READ;
				else
					state |= READY;
				return ;
			}
			return _set_error(404);
		}
	}
	else if (request.method.compare("DELETE") == 0)
	{
		if (unlink(path.c_str()) == 0)
		{
			response.status = 204; // DELETE  OK but no more information to send
			state |= READY;
			return ;
		}
		return _set_error(404);
	}
	else
	{
		response.status = 200;
		state |= READY;
	}
}

void Socket_client::_set_error(short code)
{
	response.status = code;
	//if ((route.error_page[to_string(code)]).size() > 0)
	//{
//		if (request.method.compare("GET") == 0 || request.method.compare("HEAD") == 0)
//			response.status = 301;
//		else
//			response.status = 308;
//		response.location = route.error_page[to_string(code)];
//		state |= READY;
//		return ;
//	}
//	else
//	{
		route.error_page.erase(to_string(code));
		if (request.method.compare("HEAD") != 0)
			response.body = "tmp";//default_page[code];
		response.content_length = 5;//default_page[code].size();
		response.content_type = "text/html";
		state |= READY;
		return ;
//	}
}

size_t Socket_client::_get_file_size(int fd)
{
	off_t	size;
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	return size;
}

bool Socket_client::_is_dir(const char* path)
{
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}
