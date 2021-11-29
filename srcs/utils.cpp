#include "utils.hpp"

std::string _ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \r\t");
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string _rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(" \r\t");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string _tolower(const std::string & str)
{
	std::string newstr(str);
	for (size_t i = 0; i < str.size(); i++)
		if (str[i] >= 'A' && str[i] <= 'Z')
			newstr[i] = str[i] + 32;
	return newstr;
}

ssize_t _hexstr_to_int(std::string const & hexstr) {
	size_t s;   
	if (!isxdigit(hexstr[0]))
		return -1;
	std::stringstream ss;
	ss << std::hex << hexstr;
	ss >> s;
	return s;
}

std::string  _statetostr(e_http_state st) {
	switch (st) {
		case HEADERS:
			return "HEADERS";
		case BODY:
			return "BODY";
		case RESPONSE:
			return "RESPONSE";
		default:
			return "ERROR";
	}
}

