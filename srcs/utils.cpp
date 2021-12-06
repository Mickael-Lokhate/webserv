#include "utils.hpp"

bool	is_number(const std::string& s)
{
	for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
		if (!std::isdigit(*it))
			return false;
	return true;
}

std::vector<std::string> split(const std::string & tosplit, char delim)
{
	std::string buf;
	std::stringstream ss(tosplit);

	std::vector<std::string> tokens;

	while (getline(ss, buf, delim))
		if (!buf.empty())
			tokens.push_back(buf);

	return tokens;

}

void	syntax_error(unsigned int line, const std::string &c)
{
	std::string str = "Syntax error near '" + c + "' at line " + to_string(line); 
	throw std::runtime_error(str);
}

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

std::string _int_to_hexstr(int nb) {
	std::string s;
	std::stringstream ss;
	ss << nb;
	ss >> std::hex >> s;
	return s;
}


