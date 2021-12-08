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
	fd_write(-1),
	closed(false)
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
	action = ref.action;
	closed = ref.closed;
	return *this;
}

void Socket_client::what_state(void) const
{
	std::cout << "----- Socket_client's state : -----" << std::endl;
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
	if (state & NEED_READ)
		std::cout << "NEED_READ" << std::endl;
	if (state & NEED_WRITE)
		std::cout << "NEED_WRITE" << std::endl;
	if (state & READY)
		std::cout << "READY" << std::endl;
	if (state & ERROR)
		std::cout << "ERROR" << std::endl;
	std::cout << "----- end state : -----" << std::endl;
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
	what_state();
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
	std::string					path = route.root.first + request.uri;
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
	response.body.append("Index of " + request.uri + "</title>\r\n\t</head>\r\n\t<body>\r\n\t" +
				"<h1>Index of " + request.uri + "</h1>\r\n\t<table>\r\n");
	response.body.append(col);
	response.body.append(sep);
	while ((dp = readdir(d)) != NULL)
	{
		if (!strftime(buf, 100, "%a, %d %b %y %T GMT", gmtime(&inf.st_mtimespec.tv_sec)))
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
					"<td align=\"right\">" + to_string(inf.st_size) + "</td></tr>\r\n");
	}
	response.body.append(sep);
	response.body.append(bot);
	closedir(d);
	state = READY;
	response.status = 200;
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
	cgi.server_name = "SERVER_NAME=" + server->server_name[0];
	cgi.script_filename = "SCRIPT_FILENAME=" + route.cgi;
	cgi.path_info = "PATH_INFO=" + _delete_query(request.uri);
	cgi.http_cookie = "HTTP_COOKIE=" + request.headers["cookie"];

	/* extract X- and HTTP- headers */
	for (std::map<std::string, std::string>::iterator it =
		request.headers.begin(); it != request.headers.end(); it++)
	{
		std::string header = _toupper(it->first);
		if (header.find("HTTP-") == 0 || header.find("X-") == 0)
		{
			cgi.special_headers.push_back(std::string("HTTP_") + header);

			std::string & back = cgi.special_headers.back();
			for (size_t i = 0; i < back.size(); i++)
				if (back[i] == '-')
					back[i] = '_';
			back += ("=" + it->second);
			cgi.envp.push_back(back.begin().base());
		}
	}

	/* static environment */
	cgi.envp.push_back(cgi.server_protocol.begin().base());
	cgi.envp.push_back(cgi.request_scheme.begin().base());
	cgi.envp.push_back(cgi.gateway_interface.begin().base());
	cgi.envp.push_back(cgi.server_software.begin().base());
	/* ------------------ */
	cgi.envp.push_back(cgi.path.begin().base());
	cgi.envp.push_back(cgi.pwd.begin().base());
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
	cgi.envp.push_back(cgi.http_cookie.begin().base());
	cgi.envp.push_back(NULL);
}

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
	static short closed_status[] = {400, 501, 413};
	static const int size = (sizeof(closed_status)/sizeof(short));
	response.status = _status;
	state = _state;
	closed = std::find(closed_status, closed_status + size, response.status) != (closed_status + size);
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
		_update_stat(ROUTE | ERROR, 400);
		return;
	}
	while (buffer_recv[request.method.size() + spaces] == ' ' )
		++spaces;
	found = buffer_recv.find(" ", request.method.size() + spaces);
	if (found == std::string::npos) {
		_update_stat(ROUTE | ERROR, 400);
		return ;
	}
	request.uri = buffer_recv.substr(request.method.size() + spaces,
		   					found - (request.method.size() + spaces));
	if (!is_valid_uri(request.uri)) {
		_update_stat(ROUTE | ERROR, 400);
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
		_update_stat(ROUTE | ERROR, 400);
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
				_update_stat(ROUTE | ERROR, 400);
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
				_update_stat(ROUTE | ERROR, 400);
				return;
			}
			request.host = val;
		}
		else if (key == "content-length")
		{
			/* Content-length duplicate */
			if (request.content_length != -1 && 
				request.method != "GET") 
			{
				_update_stat(ROUTE | ERROR, 400);
				return;
			}
			try 
			{
				request.content_length = _extract_content_length(val); 
				if (request.content_length < 0)
					throw std::invalid_argument("invalid content length");
			}
			catch (std::exception & e)
			{
				_update_stat(ROUTE | ERROR, 400);
				return;
			}
		}
		else if (key == "transfer-encoding")
		{
			/* Transfer-encoding duplicate */
			if (request.chunked)
			{
				_update_stat(ROUTE | ERROR, 400);
				return;
			}
			if (val == "chunked")
				request.chunked = true;
			else 
			{
				_update_stat(ROUTE | ERROR, 501);
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
		size_chunck = _hexstr_to_ssize(buffer_recv.substr(cursor, pos_delim));
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
			_update_stat(RESPONSE | ERROR, 400);
			return ;
		}
	}
	if (!response.status && request.content_length > _stol(route.max_body_size)) {
		_update_stat(RESPONSE | ERROR, 413);
		return;
	}	
	state = BODY;
}


void Socket_client::prepare_response() {
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
		server = socket_server->servers[0]; 
		route = server->choose_route(request.uri);
	}
#ifdef DEBUG1
	route.what();
	server->what();
#endif
	
	if (closed) {
		state = RESPONSE;
		state |= ERROR;
		return ;
	}
	state = BODY;
}

void Socket_client::_set_error(short code)
{
	if (state & ERROR) {
		response.body = default_pages[code];
		state = READY;
		response.read_end = true;
		response.content_length = response.body.size();
		response.content_type = "text/html";
		response.status = code;
	}
	else {
		_update_stat(ERROR, code);
		process_response();
	}
}

void Socket_client::_set_action() {
	if (!route.return_.first.empty())
		action = ACTION_RETURN;
	else if (!route.upload.empty() && (request.method.compare("POST") == 0 || request.method.compare("PUT") == 0))
		action = ACTION_UPLOAD;
	else if (!route.cgi.empty())
		action = ACTION_CGI;
	else
		action = ACTION_NORMAL;
}

void Socket_client::process_response() {	
	if (state & ERROR)
	{
		if (route.error_page.find(to_string(response.status)) != route.error_page.end())
		{
			route = server->choose_route(route.error_page[to_string(response.status)]);
			request.method = "GET";
			request.uri = route.error_page[to_string(response.status)];
			request.body.clear();
		}
		else 
		{
			_set_error(response.status);
			return ;
		}
	}
	_set_action();
	if (route.limit_except.size() && find(route.limit_except.begin(),
		route.limit_except.end(), request.method) == route.limit_except.end())
		_set_error(405);
	else if (action == ACTION_RETURN)
		_process_return();
	else if (action == ACTION_UPLOAD)
		_process_upload();
	else if (action == ACTION_CGI)
		_process_cgi();
	else
		_process_normal();
#ifdef DEBUG1
	std::cout << "--RESPONSE--" << std::endl;
	response.what();
#endif
}

void Socket_client::process_body_response()
{
	std::string size_str;
	size_t	size_chunked;
	const int SIZE_CH = 8184;

	if (response.chunked) {
		while(!response.body.empty()) {
			if (!response.head_send)
				size_chunked = std::min(response.body.size(), (size_t)SIZE_CH - buffer_send.size());
			else	
				size_chunked = std::min(response.body.size(), (size_t)SIZE_CH);
			buffer_send.append(_size_to_hexstr(size_chunked) + CRLF);
			buffer_send.append(response.body.substr(0, size_chunked));
			buffer_send.append(CRLF);
			response.body.erase(0, size_chunked);
			if (response.body.size() < SIZE_CH && !response.read_end)
				break;
			if (response.body.empty())
				buffer_send.append(std::string(CRLF) + "0" + CRLF);
		}
	}
	else {
		buffer_send.append(response.body);
		response.body.clear();
	}
	response.head_send = true;
}

void Socket_client::process_header_response()
{
	char			buf[100];
	struct timeval	now;

	if (action != ACTION_CGI)
	{
		/* status line */
		buffer_send.append("HTTP/1.1 " + to_string(response.status) +
				+ " " + status_msgs[response.status] + CRLF);
		/* common headers */
		buffer_send.append(std::string("Connection: ") +
				(closed ? "closed" : "keep-alive") + CRLF);

		buffer_send.append(std::string("Content-Type: ") +
				response.content_type + CRLF);
		buffer_send.append(std::string("Server: ") +
				"webserv/v0.1" + CRLF);
		if (gettimeofday(&now, NULL) == -1)
			throw std::runtime_error(strerror(errno));
		if (!strftime(buf, 100, "%a, %d %b %y %T GMT", gmtime(&now.tv_sec)))
			throw std::runtime_error("strftime");
		buffer_send.append(std::string("Date: ") + buf + CRLF);
		if (response.chunked)
			buffer_send.append(std::string("Transfer-Encoding:") +
					"chunked" + CRLF);
		else 
			buffer_send.append(std::string("Content-Length: ") +
					to_string(response.content_length) + CRLF);
		if (action == ACTION_RETURN)
			buffer_send.append(std::string("Location: ") +
					response.location + CRLF);
		/* custom headers */
		for (std::map<std::string, std::string>::iterator it =
			response.headers.begin(); it != response.headers.end(); it++)
			buffer_send.append(it->first + ": " + it->second + CRLF);
		buffer_send.append(CRLF);
	}
	else 
	{
		size_t found = response.body.find("\n\n");
		if (found == std::string::npos)
			buffer_send.append(std::string("HTTP/1.1") +
					" 502 " + status_msgs[502] + CRLF);
		/* if no content-length nor content-type, 502 */
		else if (response.body.find("Content-Type", found) == std::string::npos &&
				(response.body.find("Content-Length", found) == std::string::npos))
			buffer_send.append(std::string("HTTP/1.1") +
					" 502 " + status_msgs[502] + CRLF);
		else 
			buffer_send.append(std::string("HTTP/1.1") +
					" 200 " + status_msgs[200] + CRLF);
		buffer_send.append(std::string("Transfer-Encoding: ") + "chunked" + CRLF);
		response.chunked = true;
		buffer_send.append(std::string("Server: ") + "webserv/v0.1" + CRLF);
		if (gettimeofday(&now, NULL) == -1)
			throw std::runtime_error(strerror(errno));
		if (!strftime(buf, 100, "%a, %d %b %y %T GMT", gmtime(&now.tv_sec)))
			throw std::runtime_error("strftime");
		buffer_send.append(std::string("Date: ") + buf + CRLF);
		buffer_send.append(std::string("Connection: keep-alive") + CRLF);
		response.body.erase(0, found);
	}
}

void Socket_client::fetch_response()
{
	if (!response.head_send) {
		process_header_response();
	}
	process_body_response();
}

void Socket_client::_process_cgi() {
	_setup_cgi();
	try { _exec_cgi(); }
	catch (...) 
	{
		_set_error(500);
		return ;
	}
	state |= NEED_WRITE;
	state |= NEED_READ;
}

void Socket_client::_process_return()
{
	response.status = to_number<short>(route.return_.first);
	response.location = route.return_.second;
	state = READY;
}

void Socket_client::_process_upload()
{
	std::string file = route.upload;
	size_t		pos = request.uri.find_last_of("/");
	std::string tmp_filename = request.uri.substr(pos, request.uri.size() - pos);
	file += tmp_filename;

	if (!_is_dir(route.upload.c_str())) 
		return _set_error(500);
	if ((fd_write = open(file.c_str(), O_WRONLY | O_TRUNC |
										O_NONBLOCK)) != -1)
	{
		state = NEED_WRITE;
		/* 204 No content */
		response.status = 204; 
		return ;
	}
	else if (errno == ENOENT && ((fd_write = open(file.c_str(),
			O_WRONLY | O_CREAT | O_NONBLOCK, 0600)) != -1))
	{
		state = NEED_WRITE;
		/* 201 Created */
		response.status = 201; 
		return ;
	}
	/*
	 * We should check if we got an error on permission or a
	 * system error which should be marked as error 500
	 */
	_set_error(403);
}

void Socket_client::_process_normal()
{
	std::string path = request.uri;
	if (!route.root.first.empty())
	{
		if (route.root.second)
			path = route.root.first;
		else
			path.insert(0, route.root.first);
	}
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
						if (!(state & ERROR))
							response.status = 200;
						response.content_length = _get_file_size(fd_read);
						response.content_type = _get_file_mime(tmp_path);
						if (request.method.compare("GET") == 0)
							state |= NEED_READ;
						else
							state = READY;
						return ;
					}
				}
				// a tester la difference entre CA
				if (route.autoindex.compare("on") == 0) 
				{
					try { generate_directory_listing(); }
					catch (...) { _set_error(500); }
					return ;
				}
				return _set_error(404);
			}
			// ET CA
			if (route.autoindex.compare("on") == 0) {
				try { generate_directory_listing(); }
				catch (...) { _set_error(500); }
				return ;
			}
			return _set_error(403);
		}
		else
		{
			if ((fd_read = open(path.c_str(), O_RDONLY | O_NONBLOCK)) != -1)
			{
				if (!(state & ERROR))
					response.status = 200;
				response.content_length = _get_file_size(fd_read);
				response.content_type = _get_file_mime(path);
				if (request.method.compare("GET") == 0)
					state |= NEED_READ;
				else
					state = READY;
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
			state = READY;
			return ;
		}
		return _set_error(404);
	}
	else
	{
		response.status = 200;
		state = READY;
	}
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
