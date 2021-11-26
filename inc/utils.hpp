#pragma once
#include <vector>
#include <string>
#include <sstream>  
#include <iostream>

bool						is_number(const std::string& s);
std::vector<std::string>	split(const std::string& to_split, char delim);

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
