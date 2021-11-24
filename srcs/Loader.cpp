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

void	Loader::_parse_config(std::vector<Server> & servers)
{
	std::cout << "Configuration parsing start" << std::endl;
	std::string	line;
	while (getline(_config_file, line))
	{
		line = _trim(line);
		if (line.empty() || line[0] == '#')
			continue ;
		_config_tab.push_back(line);
	}
	std::vector<std::string>::iterator	beg = _config_tab.begin();
	std::vector<std::string> tmp_split;
	for (; beg != _config_tab.end(); beg++)
	{
		if ((*beg).find("server") != std::string::npos &&
				((*beg)[(*beg).size() - 1] == '{' || (*(beg + 1)).compare("{") == 0 ))
		{
			++beg;
			if ((*beg).compare("{") == 0)
				++beg;
			Server	new_server;
			Route	default_route;
			std::vector<std::vector<std::string>::iterator> loc_it;
			while (beg != _config_tab.end() && ((*beg).compare("}")) != 0)
			{
				if ((*beg).find("listen") != std::string::npos)
				{
					tmp_split = split(*beg, ' ');
					if (tmp_split.size() <= 1 || tmp_split.size() > 3)
						throw std::string("Syntax error");
					if (tmp_split.size() == 2)
						new_server.port = tmp_split.at(1);
					else
					{
						new_server.address = tmp_split.at(1);
						new_server.port = tmp_split.at(2);
					}
					tmp_split.clear();
				}
				else if ((*beg).find("server_name") != std::string::npos)
				{
					tmp_split = split(*beg, ' ');
					if (tmp_split.size() <= 1)
						throw std::string("Syntax error");
					new_server.server_name = tmp_split.at(1);
				}
				else if ((*beg).find("root") != std::string::npos)
				{
					tmp_split = split(*beg, ' ');
					if (tmp_split.size() <= 1)
						throw std::string("Syntax error");
					default_route.root.first = tmp_split.at(1);
					if (tmp_split.size() == 3)
					{
						if (tmp_split.at(2).compare("alias") == 0)
							default_route.root.second = true;
						else
							throw std::string("Syntax error");
					}
				}
				else if ((*beg).find("autoindex") != std::string::npos)
				{
					tmp_split = split(*beg, ' ');
					if (tmp_split.size() != 2)
						throw std::string("Syntax error");
					default_route.autoindex = tmp_split.at(1);
				}
				else if ((*beg).find("client_max_body_size") != std::string::npos)
				{
					//TODO: Error management
					tmp_split = split(*beg, ' ');
					if (tmp_split.size() != 2)
						throw std::string("Syntax error");
					default_route.max_body_size = tmp_split.at(1);
				}
				else if ((*beg).find("return") != std::string::npos)
				{
					tmp_split = split(*beg, ' ');
					if (tmp_split.size() != 3)
						throw std::string("Syntax error");
					default_route.return_.first = tmp_split.at(1);
					default_route.return_.second = tmp_split.at(2);
				}
				else if ((*beg).find("index") != std::string::npos)
				{
					tmp_split = split(*beg, ' ');
					if (tmp_split.size() < 2)
						throw std::string("Syntax error");
					default_route.index.clear();
					for (std::vector<std::string>::iterator tmp_it = tmp_split.begin() + 1; tmp_it != tmp_split.end(); ++tmp_it)
						default_route.index.push_back(*tmp_it);
				}
				else if ((*beg).find("location") != std::string::npos)
				{
					loc_it.push_back(beg);
					while (beg != _config_tab.end() && (*beg).compare("}") != 0)
						++beg;
				}
				else
					throw std::string("Syntax error");
				++beg;
			}
			new_server.add_route(default_route);
			_create_route(new_server, default_route, loc_it);
			new_server.what();
		}
		else
			throw std::string("Syntax error");
	}
}

void	Loader::_parse_server(Server & new_server)
{
	std::cout << "Parse  of a server start" << std::endl;
	// Setup tous les attributs du server rencontre
	// Setup les attributs de new_route_conf avec les valeurs du fichier de conf ou NULL 
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
		new_route.root.first = default_route.root.first;
		new_route.root.second = default_route.root.second;
		new_route.autoindex = default_route.autoindex;
		new_route.max_body_size = default_route.max_body_size;
		new_route.return_.first = default_route.return_.first;
		new_route.return_.second = default_route.return_.second;
		new_route.index = default_route.index;
		std::vector<std::string> tmp_split = split(*(*beg), ' ');
		std::vector<std::string>::iterator	line = ++(*beg);
		if (tmp_split.size() < 2)
			throw std::string("Syntax error");
		if (tmp_split.at(1).find(".") != std::string::npos)
		{
			tmp_split = split(tmp_split.at(1), '.');
			if (tmp_split.size() != 2)
				throw std::string("Syntax error");
			new_route.location = tmp_split.at(0);
			new_route.ext = tmp_split.at(1);
		}
		else
			new_route.location = tmp_split.at(1);
		while ((*line).compare("}") != 0)
		{
			if ((*line).find("root") != std::string::npos)
			{
				tmp_split = split(*line, ' ');
				if (tmp_split.size() <= 1)
					throw std::string("Syntax error");
				new_route.root.first = tmp_split.at(1);
				if (tmp_split.size() == 3)
				{
					if (tmp_split.at(2).compare("alias") == 0)
						new_route.root.second = true;
					else
						throw std::string("Syntax error");
				}
			}
			else if ((*line).find("autoindex") != std::string::npos)
			{
				tmp_split = split((*line), ' ');
				if (tmp_split.size() != 2)
					throw std::string("Syntax error");
				new_route.autoindex = tmp_split.at(1);
			}
			else if ((*line).find("client_max_body_size") != std::string::npos)
			{
				//TODO: Error management
				tmp_split = split((*line), ' ');
				if (tmp_split.size() != 2)
					throw std::string("Syntax error");
				new_route.max_body_size = tmp_split.at(1);
			}
			else if ((*line).find("return") != std::string::npos)
			{
				tmp_split = split((*line), ' ');
				if (tmp_split.size() != 3)
					throw std::string("Syntax error");
				new_route.return_.first = tmp_split.at(1);
				new_route.return_.second = tmp_split.at(2);
			}
			else if ((*line).find("index") != std::string::npos)
			{
				tmp_split = split((*line), ' ');
				if (tmp_split.size() < 2)
					throw std::string("Syntax error");
				new_route.index.clear();
				for (std::vector<std::string>::iterator tmp_it = tmp_split.begin() + 1; tmp_it != tmp_split.end(); ++tmp_it)
					new_route.index.push_back(*tmp_it);
			}
			else if ((*line).find("limit_except") != std::string::npos)
			{
				tmp_split = split((*line), ' ');
				if (tmp_split.size() < 2)
					throw std::string("Syntax error");
				//TODO: modify Route, not only 1 method
				//if (tmp_split.at(1) != "GET" || tmp_split.at(1) != "POST"
				//		|| tmp_split.at(1) != "PUT" || tmp_split.at(1) != "HEAD"
				//		|| tmp_split.at(1) != "DELETE")
				//	throw std::string("Syntax error");
				new_route.limit_except.clear();
				for (int i = 1; i < tmp_split.size(); i++)
					new_route.limit_except.push_back(tmp_split.at(i));
			}
			else if ((*line).find("cgi") != std::string::npos)
			{
				tmp_split = split((*line), ' ');
				if (tmp_split.size() != 2)
					throw std::string("Syntax error");
				new_route.cgi = tmp_split.at(1);
			}
			else if ((*line).find("upload") != std::string::npos)
			{
				tmp_split = split((*line), ' ');
				if (tmp_split.size() != 2)
					throw std::string("Syntax error");
				new_route.upload = tmp_split.at(1);
			}
			else if ((*line).find("location") != std::string::npos)
			{
				while ((*line).compare("}") != 0)
					++(line);
			}
			++(line);
		}
		server.add_route(new_route);
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
