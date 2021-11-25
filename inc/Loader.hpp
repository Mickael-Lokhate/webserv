#pragma once

#include <fstream>
#include <vector>
#include <algorithm>
#include "Server.hpp"
#include "Route.hpp"

#define SYNTAX_ERROR std::runtime_error("Syntax error in configuration file")

class Loader {

public:
	typedef	std::vector<std::string>						t_vector_string;
	typedef std::vector<std::vector<std::string>::iterator> t_vector_iterator;
	typedef	std::vector<std::string>::iterator				t_vector_string_iterator;

public:
	Loader(std::ifstream & config);
	~Loader(void);

	void	what(void) const;
	void	add_servers(std::vector<Server> & servers);

private:
	std::ifstream & _config_file; 
	std::vector<std::string> _config_tab;

	Loader(void);
	void	_parse_config(std::vector<Server> & servers);
	void	_print_config(std::string curr_line) const;
	void	_create_route(Server & server, Route &default_route, std::vector<std::vector<std::string>::iterator>& loc_it);	
	void	_fill_config_tab(void);

	void	_treat_listen(t_vector_string split_line, Server &new_server);
	void	_treat_server_name(t_vector_string split_line, Server &new_server);

	void	_treat_root(t_vector_string split_line, Route &default_route);
	void	_treat_autoindex(t_vector_string split_line, Route &default_route);
	void	_treat_max_body_size(t_vector_string split_line, Route &default_route);
	void	_treat_return(t_vector_string split_line, Route &default_route);
	void	_treat_index(t_vector_string split_line, Route &default_route);
	void	_treat_limit_except(t_vector_string split_line, Route &default_route);
	void	_treat_cgi(t_vector_string split_line, Route &default_route);
	void	_treat_upload(t_vector_string split_line, Route &default_route);
	void	_get_location_name(t_vector_string &split_line, Route &default_route);

	void	_treat_location(t_vector_iterator &location_iterator, t_vector_string_iterator &it);

	void	_treatment_server(t_vector_string_iterator &begin, Server &new_server, Route &default_route, t_vector_iterator &location_iterator);
	void	_treatment_location(t_vector_string_iterator &begin, Route &default_route, t_vector_iterator &location_iterator);
	
	std::string	_trim(const std::string &s);
	std::string	_ltrim(const std::string &s);
	std::string	_rtrim(const std::string &s);
};

