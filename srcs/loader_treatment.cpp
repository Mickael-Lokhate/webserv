#include "Loader.hpp"

void	Loader::_treatment_common(t_vector_string split_line, t_vector_string_iterator &begin, Route &default_route, t_vector_iterator &location_iterator)
{
	if (split_line.at(0).compare("root") == 0)
		_treat_root(split_line, default_route);
	else if (split_line.at(0).compare("autoindex") == 0)
		_treat_autoindex(split_line, default_route);
	else if (split_line.at(0).compare("client_max_body_size") == 0)
		_treat_max_body_size(split_line, default_route);
	else if (split_line.at(0).compare("return") == 0)
		_treat_return(split_line, default_route);
	else if (split_line.at(0).compare("index") == 0)
		_treat_index(split_line, default_route);
	else if (split_line.at(0).compare("error_page") == 0)
		_treat_error_page(split_line, default_route);
	else if (split_line.at(0).compare("location") == 0)
		_treat_location(location_iterator, begin);
	else
		throw SYNTAX_ERROR;
}

void	Loader::_treatment_location(t_vector_string_iterator &begin, Route &default_route, t_vector_iterator &location_iterator)
{
	t_vector_string	tmp_split = split(*begin, ' ');
	if (tmp_split.at(0).compare("limit_except") == 0)
		_treat_limit_except(tmp_split, default_route);
	else if (tmp_split.at(0).compare("cgi") == 0)
		_treat_cgi(tmp_split, default_route);
	else if (tmp_split.at(0).compare("upload") == 0)
		_treat_upload(tmp_split, default_route);
	else
		_treatment_common(tmp_split, begin, default_route, location_iterator);
}

void	Loader::_treatment_server(t_vector_string_iterator &begin, Server &new_server, Route &default_route, t_vector_iterator &location_iterator)
{
	t_vector_string	tmp_split = split(*begin, ' ');

	if (tmp_split.at(0).compare("listen") == 0)
		_treat_listen(tmp_split, new_server);
	else if (tmp_split.at(0).compare("server_name") == 0)
		_treat_server_name(tmp_split, new_server);
	else
		_treatment_common(tmp_split, begin, default_route, location_iterator);
}

void	Loader::_treat_listen(t_vector_string split_line, Server &new_server)
{
	if (split_line.size() <= 1 || split_line.size() > 3)
		throw SYNTAX_ERROR;
	if (split_line.size() == 2)
	{
		if (!is_number(split_line.at(1)))
			throw SYNTAX_ERROR;
		new_server.port = split_line.at(1);
	}
	else
	{
		if (!is_number(split_line.at(2)))
			throw SYNTAX_ERROR;
		new_server.address = split_line.at(1);
		new_server.port = split_line.at(2);
	}
}

void	Loader::_treat_server_name(t_vector_string split_line, Server &new_server)
{
	if (split_line.size() < 2)
		throw SYNTAX_ERROR;
	new_server.server_name.clear();
	for (t_vector_string_iterator it = split_line.begin() + 1; it != split_line.end(); it++)
		new_server.server_name.push_back(*it);
}

void	Loader::_treat_root(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() < 2 || split_line.size() > 3)
		throw SYNTAX_ERROR;
	if (split_line.size() == 2 && split_line.at(1).compare("alias") == 0)
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
	if (!is_number(split_line.at(1)))
		throw SYNTAX_ERROR;
	default_route.max_body_size = split_line.at(1);
}

void	Loader::_treat_return(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() < 2 || split_line.size() > 3)
		throw SYNTAX_ERROR;
	if (split_line.size() == 2)
	{
		default_route.return_.first = "";
		default_route.return_.second = split_line.at(1);
	}
	else
	{
		if (!is_number(split_line.at(1)))
			throw SYNTAX_ERROR;
		default_route.return_.first = split_line.at(1);
		default_route.return_.second = split_line.at(2);
	}
}

void	Loader::_treat_index(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() < 2)
		throw SYNTAX_ERROR;
	default_route.index.clear();
	for (t_vector_string_iterator tmp_it = split_line.begin() + 1; tmp_it != split_line.end(); ++tmp_it)
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

void	Loader::_treat_error_page(t_vector_string split_line, Route &default_route)
{
	if (split_line.size() < 3)
		throw SYNTAX_ERROR;
	for (size_t i = 1; i < split_line.size() - 1; ++i)
	{
		if (!is_number(split_line.at(i)))
			throw SYNTAX_ERROR;
		default_route.error_page.insert(std::make_pair(split_line.at(i), split_line.at(split_line.size() - 1)));
	}
}

void	Loader::_get_location_name(t_vector_string  &split_line, Route &default_route)
{
	if (split_line.size() < 2 || split_line.size() > 3)
		throw SYNTAX_ERROR;
	if (split_line.size() == 2 && (split_line.at(1).compare("{") == 0))
		throw SYNTAX_ERROR;
	if (split_line.size() == 3 && (split_line.at(2).compare("{") != 0))
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
