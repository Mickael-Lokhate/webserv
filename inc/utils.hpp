#pragma once

#include <exception>
#include <iostream>
#include <sstream>

#define STRDEBUG(MSG) std::string(__FUNCTION__) + " : " + MSG + " ("  + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ")"

#define STRDEBUG0 std::string(__FUNCTION__) + " : " + strerror(errno) + " ("  + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ")"

template<class T>
void what_(T elmt) {
	elmt.what();	
}

typedef enum http_state {
	REQUEST_LINE,
	HEADERS,
	BODY,
	RESPONSE,
} e_http_state;

std::string _ltrim(const std::string &s);
std::string _rtrim(const std::string &s);
std::string _tolower(const std::string & str);
ssize_t _hexstr_to_int(std::string const & hexstr);
std::string  _statetostr(e_http_state st);
