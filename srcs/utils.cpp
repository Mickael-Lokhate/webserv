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
		tokens.push_back(buf);

	return tokens;

}
