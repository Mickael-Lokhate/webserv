#pragma once
#include <iostream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "utils.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Cgi.hpp"
#include "Socket_server.hpp"
#define DIGIT 1

class Socket_client {	

	public:

	int				fd;
	std::string		buffer_recv;
	std::string		buffer_send;
	std::string		addr;
	std::string		port;
	int				state;
	Cgi				cgi;
	Request			request;
	Response		response;
	Route			route;
    Socket_server   * socket_server;
	int				fd_read;
	int 			fd_write;
	Server			* server;

	Socket_client(int fd = -1, const std::string & addr = "no_adr", 
						const std::string & port = "no_port", 
						Socket_server * socket_server = NULL);
	Socket_client(const Socket_client & ref);
	~Socket_client(void);
	Socket_client & operator=(const Socket_client & ref);

	void process_request_line(void);
	void process_headers(void);
	void process_body(void);

	void prepare_response();
	void process_response();

	void _update_stat(int _state, short _status);

	void generate_directory_listing(void);

	bool is_valid_uri(std::string const & str);
	const std::string & check_method(void);

	bool get_ckunked_body(void);
	bool get_simple_body(void);
	void what(void) const;
	void big_what(void) const;

	private:

	void _setup_cgi(void);
	void _prepare_pipes(void);
	void _process_cgi(void);
	void _exec_cgi(void);
	void _process_error_page(void);
	void _process_return(void);
	void _process_upload(void);
	void _process_normal(void);
	void _set_error(short code);
	size_t  _get_file_size(int fd);
	bool	_is_dir(const char* path);
};
