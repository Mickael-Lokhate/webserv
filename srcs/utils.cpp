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
