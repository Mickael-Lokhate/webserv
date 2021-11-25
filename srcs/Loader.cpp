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
	std::vector<std::string>::iterator	beg;
	std::vector<std::string> tmp_split;
	
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
				tmp_split = split(*beg, ' ');
				if (tmp_split.at(0).compare("listen") == 0)
					_treat_listen(tmp_split, new_server);
				else if (tmp_split.at(0).compare("server_name") == 0)
					_treat_server_name(tmp_split, new_server);	
				else if (tmp_split.at(0).compare("root") == 0)
					_treat_root(tmp_split, default_route);
				else if (tmp_split.at(0).compare("autoindex") == 0)
					_treat_autoindex(tmp_split, default_route);	
				else if (tmp_split.at(0).compare("client_max_body_size") == 0)
					_treat_max_body_size(tmp_split, default_route); 	
				else if (tmp_split.at(0).compare("return") == 0)
					_treat_return(tmp_split, default_route);	
				else if (tmp_split.at(0).compare("index") == 0)
					_treat_index(tmp_split, default_route);	
				else if (tmp_split.at(0).compare("location") == 0)
					_treat_location(loc_it, beg);	
				else
					throw std::runtime_error("Syntax error");
				tmp_split.clear();
				if (beg + 1 != _config_tab.end())
					++beg;
				else
					throw std::runtime_error("Syntax error");
			}
			new_server.routes.clear();
			new_server.add_route(default_route);
			_create_route(new_server, default_route, loc_it);
			new_server.what();
		}
		else
			throw std::runtime_error("Syntax error");
	}
}

void	Loader::_treat_listen(t_vector_string split_line, Server &new_server)
{
	if (split_line.size() <= 1 || split_line.size() > 3)
		throw std::runtime_error("Syntax error");
	if (split_line.size() == 2)
		new_server.port = split_line.at(1);
	else
	{
		new_server.address = split_line.at(1);
		new_server.port = split_line.at(2);
	}
}

void	Loader::_treat_server_name(t_vector_string split_line, Server &new_server)
{
	if (split_line.size() <= 1)
		throw std::runtime_error("Syntax error");
	new_server.server_name = split_line.at(1);
}

void	Loader::_treat_root(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() <= 1)
		throw std::runtime_error("Syntax error");
	default_route.root.first = split_line.at(1);
	if (split_line.size() == 3)
	{
		if (split_line.at(2).compare("alias") == 0)
			default_route.root.second = true;
		else
			throw std::runtime_error("Syntax error");
	}
}
void	Loader::_treat_autoindex(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() != 2)
		throw std::runtime_error("Syntax error");
	default_route.autoindex = split_line.at(1);
}

void	Loader::_treat_max_body_size(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() != 2)
		throw std::runtime_error("Syntax error");
	default_route.max_body_size = split_line.at(1);
}

void	Loader::_treat_return(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() != 3)
		throw std::runtime_error("Syntax error");
	default_route.return_.first = split_line.at(1);
	default_route.return_.second = split_line.at(2);
}

void	Loader::_treat_index(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() < 2)
		throw std::runtime_error("Syntax error");
	default_route.index.clear();
	for (std::vector<std::string>::iterator tmp_it = split_line.begin() + 1; tmp_it != split_line.end(); ++tmp_it)
		default_route.index.push_back(*tmp_it);
}

void	Loader::_treat_location(t_vector_iterator &location_iterator, t_vector_string_iterator &it)
{
	location_iterator.push_back(it);
	while (it != _config_tab.end() && (*it).compare("}") != 0)
	{
		++it;
		if ((*it).find("location") != std::string::npos)
		{
			while (it != _config_tab.end() && (*it).compare("}") != 0)
				++it;
			if (it != _config_tab.end())
				++it;
		}
	}
}

void	Loader::_treat_limit_except(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() < 2)
		throw std::runtime_error("Syntax error");
	
	default_route.limit_except.clear();
	for (size_t i = 1; i < split_line.size(); i++)
	{
		if (split_line.at(i) != "GET" && split_line.at(i) != "POST"
			&& split_line.at(i) != "PUT" && split_line.at(i) != "HEAD"
			&& split_line.at(i) != "DELETE")	
			throw std::runtime_error("Syntax error");
		default_route.limit_except.push_back(split_line.at(i));
	}
}

void	Loader::_treat_cgi(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() != 2)
		throw std::runtime_error("Syntax error");
	default_route.cgi = split_line.at(1);
}

void	Loader::_treat_upload(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() != 2)
		throw std::runtime_error("Syntax error");
	default_route.upload = split_line.at(1);
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

void	Loader::_create_route(Server & server, Route &default_route, std::vector<std::vector<std::string>::iterator>& loc_it)
{
	for (std::vector<std::vector<std::string>::iterator>::iterator beg = loc_it.begin(); beg != loc_it.end(); ++beg)
	{
		Route new_route;
		new_route.location = default_route.location;
		new_route.root.first = default_route.root.first;
		new_route.root.second = default_route.root.second;
		new_route.autoindex = default_route.autoindex;
		new_route.max_body_size = default_route.max_body_size;
		new_route.return_.first = default_route.return_.first;
		new_route.return_.second = default_route.return_.second;
		new_route.index = default_route.index;
		std::vector<std::string> tmp_split = split(*(*beg), ' ');
		std::vector<std::string>::iterator	line = ++(*beg);
		std::vector<std::vector<std::string>::iterator> new_loc_vec;
		if (tmp_split.size() < 2)
			throw std::runtime_error("Syntax error");
		if (tmp_split.at(1).find(".") != std::string::npos)
		{
			tmp_split = split(tmp_split.at(1), '.');
			if (tmp_split.size() != 2)
				throw std::runtime_error("Syntax error");
			if (!(tmp_split.at(0).empty()))
				new_route.location = tmp_split.at(0);
			new_route.ext = tmp_split.at(1);
		}
		else
			new_route.location = tmp_split.at(1);
		while ((*line).compare("}") != 0)
		{
			tmp_split = split(*line, ' ');
			if (tmp_split.at(0).compare("root") == 0)
				_treat_root(tmp_split, new_route);	
			else if (tmp_split.at(0).compare("autoindex") == 0)
				_treat_autoindex(tmp_split, new_route);	
			else if (tmp_split.at(0).compare("client_max_body_size") == 0)
				_treat_max_body_size(tmp_split, new_route);
			else if (tmp_split.at(0).compare("return") == 0)
				_treat_return(tmp_split, new_route);	
			else if (tmp_split.at(0).compare("index") == 0)
				_treat_index(tmp_split, new_route);	
			else if (tmp_split.at(0).compare("limit_except") == 0)
				_treat_limit_except(tmp_split, new_route);
			else if (tmp_split.at(0).compare("cgi") == 0)
				_treat_cgi(tmp_split, new_route);
			else if (tmp_split.at(0).compare("upload") == 0)
				_treat_upload(tmp_split, new_route);
			else if (tmp_split.at(0).compare("location") == 0)
				_treat_location(new_loc_vec, line);
			else
				throw std::runtime_error("Syntax error");
			++(line);
			tmp_split.clear();
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
 
std::string Loader::_trim(const std::string &s) {
    return _rtrim(_ltrim(s));
}
