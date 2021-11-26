#include "utils.hpp"

bool	is_number(const std::string& s)
{
	for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
		if (!std::isdigit(*it))
			return false;
	return true;
}
