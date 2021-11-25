#include "Loader.hpp"

Loader::Loader(std::ifstream& config) : _config_file(config)
{
	std::cout << "Loader start" << std::endl;
}

Loader::~Loader(void)
{
	std::cout << "Loader destruction" << std::endl;
}

void	Loader::add_servers(std::vector<Server> & servers)
{
	_parse_config(servers);
}

void	Loader::_fill_config_tab(void)
{
	std::string	line;
	while (getline(_config_file, line))
	{
		line = _trim(line);
		if (line.empty() || line[0] == '#')
			continue ;
		_config_tab.push_back(line);
	}
}

void	Loader::_parse_config(std::vector<Server> & servers)
{
	t_vector_string_iterator	beg;
	
	_fill_config_tab();	
	for (beg = _config_tab.begin(); beg != _config_tab.end(); beg++)
	{
		if ((*beg).find("server") != std::string::npos
				&& ((*beg)[(*beg).size() - 1] == '{'
				|| (*(++beg)).compare("{") == 0 ))
		{
			++beg;

			Server				new_server;
			Route				default_route;
			t_vector_iterator	loc_it;
			while (beg != _config_tab.end() && ((*beg).compare("}")) != 0)
			{
				_treatment_server(beg, new_server, default_route, loc_it);
				if (beg + 1 != _config_tab.end())
					++beg;
				else
					throw SYNTAX_ERROR;
			}
			new_server.routes.clear();
			new_server.add_route(default_route);
			_create_route(new_server, default_route, loc_it);
			new_server.what();
			servers.push_back(new_server);
		}
		else
			throw SYNTAX_ERROR;
	}
}

void	Loader::_create_route(Server & server, Route &default_route, t_vector_iterator& loc_it)
{
	for (t_vector_iterator::iterator beg = loc_it.begin(); beg != loc_it.end(); ++beg)
	{
		Route 						new_route(default_route);
		t_vector_string				tmp_split = split(*(*beg), ' ');
		t_vector_string_iterator	line = ++(*beg);
		t_vector_iterator			 new_loc_vec;

		_get_location_name(tmp_split, new_route);
		while ((*line).compare("}") != 0)
		{
			_treatment_location(line, new_route, new_loc_vec);
			if ((line + 1) != _config_tab.end())
				++(line);
			else
				throw SYNTAX_ERROR;
		}
		server.add_route(new_route);
		_create_route(server, new_route, new_loc_vec);
	}
}

std::string Loader::_ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \r\t");
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string Loader::_rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(" \r\t");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string	Loader::_trim_comments(const std::string &s)
{
	size_t	pos = s.find("#");
	return (pos == std::string::npos) ? s : s.substr(0, pos);
}
 
std::string Loader::_trim(const std::string &s) {
    return _rtrim(_ltrim(_trim_comments(s)));
}

void	Loader::_print_config(std::string curr_line) const {
	std::cout << curr_line << std::endl;
}

void	Loader::what(void) const
{
	std::string tmp;

	std::cout << "Loader content : " << std::endl;
	for_each(_config_tab.begin(), _config_tab.end(), std::bind1st(std::mem_fun(&Loader::_print_config), this));	
}
