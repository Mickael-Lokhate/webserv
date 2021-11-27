#pragma once
#include <vector>
#include <string>
#include <sstream>  
#include <iostream>

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
