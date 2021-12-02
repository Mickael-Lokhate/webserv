#include "utils.hpp"

std::map<short, std::string> status_msgs;
std::map<short, std::string> default_pages;

void add_status_msgs() {
	status_msgs[100] = "Continue";
	status_msgs[101] = "Switching Protocols";
	status_msgs[102] = "Processing";
	status_msgs[103] = "Early Hints";
	status_msgs[200] = "OK";
	status_msgs[201] = "Created";
	status_msgs[202] = "Accepted";
	status_msgs[203] = "Non-Authoritative Information";
	status_msgs[204] = "No Content";
	status_msgs[205] = "Reset Content";
	status_msgs[206] = "Partial Content";
	status_msgs[207] = "Multi-Status";
	status_msgs[208] = "Already Reported";
	status_msgs[226] = "IM Used";
	status_msgs[300] = "Multiple Choices";
	status_msgs[301] = "Moved Permanently";
	status_msgs[302] = "Found";
	status_msgs[303] = "See Other";
	status_msgs[304] = "Not Modified";
	status_msgs[305] = "Use Proxy";
	status_msgs[307] = "Temporary Redirect";
	status_msgs[308] = "Permanent Redirect";
	status_msgs[400] = "Bad Request";
	status_msgs[401] = "Unauthorized";
	status_msgs[402] = "Payment Required";
	status_msgs[403] = "Forbidden";
	status_msgs[404] = "Not Found";
	status_msgs[405] = "Method Not Allowed";
	status_msgs[406] = "Not Acceptable";
	status_msgs[407] = "Proxy Authentication Required";
	status_msgs[408] = "Request Timeout";
	status_msgs[409] = "Conflict";
	status_msgs[410] = "Gone";
	status_msgs[411] = "Length Required";
	status_msgs[412] = "Precondition Failed";
	status_msgs[413] = "Content Too Large";
	status_msgs[414] = "URI Too Long";
	status_msgs[415] = "Unsupported Media Type";
	status_msgs[416] = "Range Not Satisfiable";
	status_msgs[417] = "Expectation Failed";
	status_msgs[421] = "Misdirected Request";
	status_msgs[422] = "Unprocessable Content";
	status_msgs[423] = "Locked";
	status_msgs[424] = "Failed Dependency";
	status_msgs[425] = "Too Early";
	status_msgs[426] = "Upgrade Required";
	status_msgs[427] = "Unassigned";
	status_msgs[428] = "Precondition Required";
	status_msgs[429] = "Too Many Requests";
	status_msgs[430] = "Unassigned";
	status_msgs[431] = "Request Header Fields Too Large";
	status_msgs[451] = "Unavailable For Legal Reasons";
	status_msgs[500] = "Internal Server Error";
	status_msgs[501] = "Not Implemented";
	status_msgs[502] = "Bad Gateway";
	status_msgs[503] = "Service Unavailable";
	status_msgs[504] = "Gateway Timeout";
	status_msgs[505] = "HTTP Version Not Supported";
	status_msgs[506] = "Variant Also Negotiates";
	status_msgs[507] = "Insufficient Storage";
	status_msgs[508] = "Loop Detected";
	status_msgs[509] = "Unassigned";
	status_msgs[510] = "Not Extended";
	status_msgs[511] = "Network Authentication Required";
}

void add_default_pages() {

default_pages[301] =
"<html>" CRLF
"<head><title>301 Moved Permanently</title></head>" CRLF
"<body>" CRLF
"<center><h1>301 Moved Permanently</h1></center>" CRLF
;


default_pages[302] =
"<html>" CRLF
"<head><title>302 Found</title></head>" CRLF
"<body>" CRLF
"<center><h1>302 Found</h1></center>" CRLF
;


default_pages[303] =
"<html>" CRLF
"<head><title>303 See Other</title></head>" CRLF
"<body>" CRLF
"<center><h1>303 See Other</h1></center>" CRLF
;


default_pages[307] =
"<html>" CRLF
"<head><title>307 Temporary Redirect</title></head>" CRLF
"<body>" CRLF
"<center><h1>307 Temporary Redirect</h1></center>" CRLF
;


default_pages[308] =
"<html>" CRLF
"<head><title>308 Permanent Redirect</title></head>" CRLF
"<body>" CRLF
"<center><h1>308 Permanent Redirect</h1></center>" CRLF
;


default_pages[400] =
"<html>" CRLF
"<head><title>400 Bad Request</title></head>" CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
;


default_pages[401] =
"<html>" CRLF
"<head><title>401 Authorization Required</title></head>" CRLF
"<body>" CRLF
"<center><h1>401 Authorization Required</h1></center>" CRLF
;


default_pages[402] =
"<html>" CRLF
"<head><title>402 Payment Required</title></head>" CRLF
"<body>" CRLF
"<center><h1>402 Payment Required</h1></center>" CRLF
;


default_pages[403] =
"<html>" CRLF
"<head><title>403 Forbidden</title></head>" CRLF
"<body>" CRLF
"<center><h1>403 Forbidden</h1></center>" CRLF
;


default_pages[404] =
"<html>" CRLF
"<head><title>404 Not Found</title></head>" CRLF
"<body>" CRLF
"<center><h1>404 Not Found</h1></center>" CRLF
;


default_pages[405] =
"<html>" CRLF
"<head><title>405 Not Allowed</title></head>" CRLF
"<body>" CRLF
"<center><h1>405 Not Allowed</h1></center>" CRLF
;


default_pages[406] =
"<html>" CRLF
"<head><title>406 Not Acceptable</title></head>" CRLF
"<body>" CRLF
"<center><h1>406 Not Acceptable</h1></center>" CRLF
;


default_pages[408] =
"<html>" CRLF
"<head><title>408 Request Time-out</title></head>" CRLF
"<body>" CRLF
"<center><h1>408 Request Time-out</h1></center>" CRLF
;


default_pages[409] =
"<html>" CRLF
"<head><title>409 Conflict</title></head>" CRLF
"<body>" CRLF
"<center><h1>409 Conflict</h1></center>" CRLF
;


default_pages[410] =
"<html>" CRLF
"<head><title>410 Gone</title></head>" CRLF
"<body>" CRLF
"<center><h1>410 Gone</h1></center>" CRLF
;


default_pages[411] =
"<html>" CRLF
"<head><title>411 Length Required</title></head>" CRLF
"<body>" CRLF
"<center><h1>411 Length Required</h1></center>" CRLF
;


default_pages[412] =
"<html>" CRLF
"<head><title>412 Precondition Failed</title></head>" CRLF
"<body>" CRLF
"<center><h1>412 Precondition Failed</h1></center>" CRLF
;


default_pages[413] =
"<html>" CRLF
"<head><title>413 Request Entity Too Large</title></head>" CRLF
"<body>" CRLF
"<center><h1>413 Request Entity Too Large</h1></center>" CRLF
;


default_pages[414] =
"<html>" CRLF
"<head><title>414 Request-URI Too Large</title></head>" CRLF
"<body>" CRLF
"<center><h1>414 Request-URI Too Large</h1></center>" CRLF
;


default_pages[415] =
"<html>" CRLF
"<head><title>415 Unsupported Media Type</title></head>" CRLF
"<body>" CRLF
"<center><h1>415 Unsupported Media Type</h1></center>" CRLF
;


default_pages[416] =
"<html>" CRLF
"<head><title>416 Requested Range Not Satisfiable</title></head>" CRLF
"<body>" CRLF
"<center><h1>416 Requested Range Not Satisfiable</h1></center>" CRLF
;


default_pages[421] =
"<html>" CRLF
"<head><title>421 Misdirected Request</title></head>" CRLF
"<body>" CRLF
"<center><h1>421 Misdirected Request</h1></center>" CRLF
;


default_pages[429] =
"<html>" CRLF
"<head><title>429 Too Many Requests</title></head>" CRLF
"<body>" CRLF
"<center><h1>429 Too Many Requests</h1></center>" CRLF
;


default_pages[494] =
"<html>" CRLF
"<head><title>400 Request Header Or Cookie Too Large</title></head>"
CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
"<center>Request Header Or Cookie Too Large</center>" CRLF
;


default_pages[495] =
"<html>" CRLF
"<head><title>400 The SSL certificate error</title></head>"
CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
"<center>The SSL certificate error</center>" CRLF
;


default_pages[496] =
"<html>" CRLF
"<head><title>400 No required SSL certificate was sent</title></head>"
CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
"<center>No required SSL certificate was sent</center>" CRLF
;


default_pages[497] =
"<html>" CRLF
"<head><title>400 The plain HTTP request was sent to HTTPS port</title></head>"
CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
"<center>The plain HTTP request was sent to HTTPS port</center>" CRLF
;


default_pages[500] =
"<html>" CRLF
"<head><title>500 Internal Server Error</title></head>" CRLF
"<body>" CRLF
"<center><h1>500 Internal Server Error</h1></center>" CRLF
;


default_pages[501] =
"<html>" CRLF
"<head><title>501 Not Implemented</title></head>" CRLF
"<body>" CRLF
"<center><h1>501 Not Implemented</h1></center>" CRLF
;


default_pages[502] =
"<html>" CRLF
"<head><title>502 Bad Gateway</title></head>" CRLF
"<body>" CRLF
"<center><h1>502 Bad Gateway</h1></center>" CRLF
;


default_pages[503] =
"<html>" CRLF
"<head><title>503 Service Temporarily Unavailable</title></head>" CRLF
"<body>" CRLF
"<center><h1>503 Service Temporarily Unavailable</h1></center>" CRLF
;


default_pages[504] =
"<html>" CRLF
"<head><title>504 Gateway Time-out</title></head>" CRLF
"<body>" CRLF
"<center><h1>504 Gateway Time-out</h1></center>" CRLF
;


default_pages[505] =
"<html>" CRLF
"<head><title>505 HTTP Version Not Supported</title></head>" CRLF
"<body>" CRLF
"<center><h1>505 HTTP Version Not Supported</h1></center>" CRLF
;


default_pages[507] =
"<html>" CRLF
"<head><title>507 Insufficient Storage</title></head>" CRLF
"<body>" CRLF
"<center><h1>507 Insufficient Storage</h1></center>" CRLF
;

}
