#pragma once
#include <vector>
#include <map>
#include <string>
#include <sstream>  
#include <iostream>
#include <exception>

#define STRDEBUG(MSG) std::string(__FUNCTION__) + " : " + MSG + " ("  + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ")"

#define STRDEBUG0 std::string(__FUNCTION__) + " : " + strerror(errno) + " ("  + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ")"

#define SPACE 1
#define COLON 1
#define CRLF "\r\n"

bool						is_number(const std::string& s);
std::vector<std::string>	split(const std::string& to_split, char delim);
void						syntax_error(unsigned int line, const std::string &c);

template<class T>
T	HexToInt(const std::string &str)
{
	if(str.size()==0)
		return 0;
	std::istringstream iss(str);
	T	result = 0;
	iss >> std::hex >> result;
	return result;
}

template < class T >
T	to_number(std::string& s)
{
	std::stringstream	ss(s);
	T					num;
	ss >> num;
	return num;
}

template < class T >
std::string	to_string(T &n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

template < class T >
bool	is_in_range(T nb, T r1, T r2)
{
	return ((nb >= r1 && nb <= r2) ? true : false);
}

template<class T>
void _what_vector(T elmt) {
	elmt.what();
}

template<class T1, class T2>
void _what_map(std::pair<T1, T2> elmt) {
	std::cout << "{" << elmt.first << ":" << elmt.second << "}\n";
}

typedef enum http_state {
	REQUEST_LINE = 1,
	HEADERS = 2,
	ROUTE = 4,
	BODY = 8,
	RESPONSE = 16,
	SETUP_CGI = 32,
	NEED_READ = 64,
	NEED_WRITE = 128,
	READY = 256,
	WAIT_CGI = 512,
	READY_CGI = 1024,
} e_http_state;

std::string _ltrim(const std::string &s);
std::string _rtrim(const std::string &s);
std::string _tolower(const std::string & str);
ssize_t _hexstr_to_int(std::string const & hexstr);
std::string  _statetostr(e_http_state st);

void add_status_msgs();
void add_default_pages();
