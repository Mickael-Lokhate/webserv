#pragma once
#include <vector>
#include <iostream>
#include "Server.hpp"

class TesterServer
{
	public:
		Server server;
		std::string loc;
	
	public:
		TesterServer();
		~TesterServer();
		void start(void);
		void formatingUri(void);
		void add_route(void);
		void delete_uri_variable(void);
		void remove_simple_dot(void);
		void format_double_dot(void);
		void decode_uri(void);
		void delete_duplicate_slash(void);
	
	private:
		void _assert_same(bool boolean);
};