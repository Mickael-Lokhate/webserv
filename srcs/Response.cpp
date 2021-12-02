#include "Response.hpp"

Response::~Response() {
}

Response::Response() : 
	status(0),
	content_length(-1),
	chunked(false),
{
}

Response::Response(Response const & ref) {
	*this = ref;
}

Response & Response::operator=(const Response & ref) {
	status = ref.status;
	headers = ref.headers;
	body = ref.body;
	content_length = ref.content_length;
	content_type = ref.content_type;
	chunked = ref.chunked;
	return *this;
}

void Response::what() const {
	std::cout << "status : {" << 	status 	<< "}\n";
	std::cout << "content_length : {" << 	content_length 	<< "}\n";
	std::cout << "content_type : {" << 	content_type 	<< "}\n";
	std::cout << "chunked : {" <<	chunked			<< "}\n";	
	std::cout << "body : {" <<	body			<< "}\n";	
	std::cout << "headers : {" << "\n";	
	std::for_each(headers.begin(), headers.end(), _what_map<std::string, std::string>);
	std::cout << "}" << "\n";	
}
