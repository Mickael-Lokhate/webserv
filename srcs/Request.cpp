#include "Request.hpp"

Request::Request() {
}

Request::~Request() {
}


Request::Request(std::string * buffer_recv) : 
	content_length(-1),
	chunked(false),
	delim("\r\n"),
	buffer_recv(buffer_recv), 
	error(0)
{
}

Request::Request(Request const & ref) {
	*this = ref;
}

Request & Request::operator=(const Request & ref) {
	method = ref.method;
	uri = ref.uri;
	host = ref.host;
	content_length = ref.content_length;
	chunked = ref.chunked;
	headers = ref.headers;
	body = ref.body;
	delim = ref.delim;
	buffer_recv = ref.buffer_recv;
	error = ref.error;
	return *this;
}

bool Request::is_valid_uri(std::string const & str) {
	if (str[0] != '/')
		return false;
	return true;
}

const std::string & Request::check_method() {
	static const std::string	methods[] = {
		"GET", "POST", "PUT", "DELETE", "err"};
	int i;

	for (i = 0; i < 4; i++)
		if (!buffer_recv->compare(0, methods[i].size(),
								methods[i]))
			break;
	return (methods[i]);
}

e_http_state Request::process_request_line()
{
	static const std::string	version = "HTTP/1.";
	size_t 						found;

	found = buffer_recv->find("\n");
	if (found == std::string::npos)
		return REQUEST_LINE;
	if (!found || (*buffer_recv)[found - 1] != '\r')
		delim = "\n";
	// empty line before request line
	if (buffer_recv->compare(0, delim.size(), delim) == 0) {
		buffer_recv->erase(0, delim.size());
		return REQUEST_LINE;
	}
	method = check_method();
	if (method == "err" || (*buffer_recv)[method.size()] != ' ')
	{
		error = 400;
		return RESPONSE;
	}
	found = buffer_recv->find(" ", method.size() + SPACE);
	if (found == std::string::npos) {
		error = 400;
		return RESPONSE;
	}
	uri = buffer_recv->substr(method.size() + SPACE,
		   					found - (method.size() + SPACE));
	if (!is_valid_uri(uri)) {
		error = 400;
		return RESPONSE;
	}
	found = buffer_recv->find(version, method.size() + uri.size() +
							SPACE * 2);
	if ((found == std::string::npos) ||
		/* doest minor version is a digit ? */
		!isdigit((*buffer_recv)[found + version.size()]) ||
		buffer_recv->compare(found + version.size() + SPACE, 
		delim.size(), delim)) {
		error = 400;
		return RESPONSE;
	}
	return HEADERS;
}

e_http_state Request::process_headers()
{
	std::string							key;
	std::string							val;
	size_t								cursor = 0;
	size_t								found;
	size_t								colon;

	for (;;)
	{
		found = buffer_recv->find(delim, cursor);
		if (found == std::string::npos)
			return HEADERS;
		if (found == cursor)
		{
			if (!host.empty())
			{
				buffer_recv->erase(0, found + delim.size());
				if (content_length == -1)
					content_length = 0;
				return BODY;
			}
			else
			{
				error = 400;
				return RESPONSE;
			}
		}
		colon = buffer_recv->find(":", cursor);
		/* ignore invalid headers */
		if (colon == std::string::npos || colon > found)
		{
			cursor = found + delim.size();
			continue ;
		}
		key = buffer_recv->substr(cursor, colon - cursor);
		key = _tolower(key);
		val = buffer_recv->substr(colon + COLON,
				found - (colon + COLON));
		val = _tolower(_ltrim(_rtrim(val)));
		if (key == "host")
		{
			/* Host duplicate */
			if (!host.empty()) {
				error = 400;
				return RESPONSE;
			}
			host = val;
		}
		else if (key == "content-length")
		{
			/* Content-length duplicate */
			if (content_length != -1) {
				error = 400;
				return RESPONSE;
			}
			try { content_length = std::stoi(val); }
			catch (std::exception & e)
			{
				error = 400;
				return RESPONSE;
			}
		}
		else if (key == "transfer-encoding")
		{
			/* Transfer-encoding duplicate */
			if (headers.find(key) != headers.end())
			{
				error = 400;
				return RESPONSE;
			}
			if (val == "chunked")
				chunked = true;
			else 
			{
				error = 400;
				return RESPONSE;
			}
		}
		else
			headers[key] = val;
		cursor = found + delim.size();
		buffer_recv->erase(0, found + delim.size());
	}
}


bool Request::get_simple_body() {
	size_t size = content_length - body.size();
	body.append(buffer_recv->substr(0, size));
	size = std::min(buffer_recv->size(), size);
	buffer_recv->erase(0, size);
	return (body.size() == (size_t)content_length);
}

bool Request::get_ckunked_body() {
	size_t length = 0;
	size_t pos_delim = 0;
	size_t cursor = 0;
	ssize_t size_chunck = -1;
	while (size_chunck != 0) {
		if ((pos_delim = buffer_recv->find(delim, cursor)) == std::string::npos) 
			break;
		// taille
		size_chunck = _hexstr_to_int(buffer_recv->substr(cursor, pos_delim));
		if (size_chunck < 0)
			throw std::logic_error("error chunked : hexa size");
		if (pos_delim + delim.size() + size_chunck + delim.size() > buffer_recv->size())
			break;
		#ifdef DEBUG1
		std::cout << "-> " << size_chunck << "\n";
		#endif
		cursor = pos_delim + delim.size();
		body.append(buffer_recv->substr(cursor, size_chunck));
		#ifdef DEBUG1
		std::cout << " {" <<  buffer_recv->substr(cursor, size_chunck) << "}\n";
		#endif
		cursor += size_chunck;
		if (buffer_recv->compare(cursor, delim.size(), delim) != 0)
			throw std::logic_error("error chunked : delimiter expected");
		cursor += delim.size();
		length += size_chunck;
	}
	buffer_recv->erase(0, cursor);
	content_length += length;
	return (size_chunck == 0);
}


e_http_state Request::process_body() {
	if (!(method == "POST" || method == "PUT"))
		return RESPONSE;
	if (!chunked) {
		if (get_simple_body())
			return RESPONSE;
	} else {
		try {
			if (get_ckunked_body())
				return RESPONSE;
		} catch(std::logic_error const & e) {
			#ifdef DEBUG
			std::cout << "body :  {" << body << "}\n";
			std::cout << "content-length :  {" << content_length << "}\n";
			std::cout << "rest : {" << buffer_recv << "}\n" ;
			std::cout << e.what() << "\n";
			#endif
			error = 400;
			return  RESPONSE;
		}
	}
	return BODY;
}

void Request::what() const {
	std::cout << "method : {" <<	method			<< "}\n";	
	std::cout << "uri : {" <<	uri				<< "}\n";	
	std::cout << "host : {" <<	host			<< "}\n";	
	std::cout << "content_length : {" << 	content_length 	<< "}\n";
	std::cout << "chunked : {" <<	chunked			<< "}\n";	
	std::cout << "body : {" <<	body			<< "}\n";	
	std::cout << "delim : {" <<	delim			<< "}\n";	
	std::cout << "buffer_recv : {" <<	*buffer_recv		<< "}\n";	
	std::cout << "error : {" <<	error			<< "}\n";	
	std::cout << "headers : {" << "\n";	
	std::for_each(headers.begin(), headers.end(), _what_map<std::string, std::string>);
	std::cout << "}" << "\n";	
}
