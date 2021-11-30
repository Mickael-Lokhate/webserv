#pragma once
#include <stdexcept>
#include <string>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <unistd.h>

class Cgi {

	public:

	Cgi(void);
	int exe(void);
	~Cgi(void);

	int input;
	int output;
	int status;
	pid_t pid;

	private:

	int in[2];
	int out[2];
};
