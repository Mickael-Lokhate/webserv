#include "Loader.hpp"

void	Loader::_treatment_location(t_vector_string_iterator &begin, Route &default_route, t_vector_iterator &location_iterator)
{
	t_vector_string	tmp_split = split(*begin, ' ');
	if (tmp_split.at(0).compare("root") == 0)
		_treat_root(tmp_split, default_route);
	else if (tmp_split.at(0).compare("autoindex") == 0)
		_treat_autoindex(tmp_split, default_route);
	else if (tmp_split.at(0).compare("client_max_body_size") == 0)
		_treat_max_body_size(tmp_split, default_route);
	else if (tmp_split.at(0).compare("return") == 0)
		_treat_return(tmp_split, default_route);
	else if (tmp_split.at(0).compare("index") == 0)
		_treat_index(tmp_split, default_route);
	else if (tmp_split.at(0).compare("limit_except") == 0)
		_treat_limit_except(tmp_split, default_route);
	else if (tmp_split.at(0).compare("cgi") == 0)
		_treat_cgi(tmp_split, default_route);
	else if (tmp_split.at(0).compare("upload") == 0)
		_treat_upload(tmp_split, default_route);
	else if (tmp_split.at(0).compare("location") == 0)
		_treat_location(location_iterator, begin);
	else
		throw SYNTAX_ERROR;
}

void	Loader::_treatment_server(t_vector_string_iterator &begin, Server &new_server, Route &default_route, t_vector_iterator &location_iterator)
{
	t_vector_string	tmp_split = split(*begin, ' ');

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
		_treat_location(location_iterator, begin);
	else
		throw SYNTAX_ERROR;
}

void	Loader::_treat_listen(t_vector_string split_line, Server &new_server)
{
	if (split_line.size() <= 1 || split_line.size() > 3)
		throw SYNTAX_ERROR;
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
		throw SYNTAX_ERROR;
	new_server.server_name = split_line.at(1);
}

void	Loader::_treat_root(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() <= 1)
		throw SYNTAX_ERROR;
	default_route.root.first = split_line.at(1);
	if (split_line.size() == 3)
	{
		if (split_line.at(2).compare("alias") == 0)
			default_route.root.second = true;
		else
			throw SYNTAX_ERROR;
	}
}
void	Loader::_treat_autoindex(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() != 2)
		throw SYNTAX_ERROR;
	default_route.autoindex = split_line.at(1);
}

void	Loader::_treat_max_body_size(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() != 2)
		throw SYNTAX_ERROR;
	default_route.max_body_size = split_line.at(1);
}

void	Loader::_treat_return(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() != 3)
		throw SYNTAX_ERROR;
	default_route.return_.first = split_line.at(1);
	default_route.return_.second = split_line.at(2);
}

void	Loader::_treat_index(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() < 2)
		throw SYNTAX_ERROR;
	default_route.index.clear();
	for (std::vector<std::string>::iterator tmp_it = split_line.begin() + 1; tmp_it != split_line.end(); ++tmp_it)
		default_route.index.push_back(*tmp_it);
}

void	Loader::_treat_location(t_vector_iterator &location_iterator, t_vector_string_iterator &it)
{
	location_iterator.push_back(it);
	while (it != _config_tab.end() && (*it).compare("}") != 0)
	{
		if ((it + 1) != _config_tab.end())
			++it;
		else
			throw SYNTAX_ERROR;
		if ((*it).find("location") != std::string::npos)
		{
			while (it != _config_tab.end() && (*it).compare("}") != 0)
				++it;
			if ((it) != _config_tab.end())
				++it;
			else
				throw SYNTAX_ERROR;
		}
	}
}

void	Loader::_treat_limit_except(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() < 2)
		throw SYNTAX_ERROR;

	default_route.limit_except.clear();
	for (size_t i = 1; i < split_line.size(); i++)
	{
		if (split_line.at(i) != "GET" && split_line.at(i) != "POST"
			&& split_line.at(i) != "PUT" && split_line.at(i) != "HEAD"
			&& split_line.at(i) != "DELETE")
			throw SYNTAX_ERROR;
		default_route.limit_except.push_back(split_line.at(i));
	}
}

void	Loader::_treat_cgi(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() != 2)
		throw SYNTAX_ERROR;
	default_route.cgi = split_line.at(1);
}

void	Loader::_treat_upload(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() != 2)
		throw SYNTAX_ERROR;
	default_route.upload = split_line.at(1);
}

void	Loader::_get_location_name(t_vector_string  &split_line, Route &default_route)
{
	if (split_line.size() < 2)
		throw SYNTAX_ERROR;
	if (split_line.at(1).find(".") != std::string::npos)
	{
		split_line = split(split_line.at(1), '.');
		if (split_line.size() != 2)
			throw SYNTAX_ERROR;
		if (!(split_line.at(0).empty()))
			default_route.location = split_line.at(0);
		default_route.ext = split_line.at(1);
	}
	else
		default_route.location = split_line.at(1);
}
