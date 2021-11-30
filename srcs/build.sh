#!/bin/sh
g++ -D DEBUG -Wall -Wextra -g -I ../inc Socket_client.cpp Socket_server.cpp Worker.cpp Request.cpp main.cpp utils.cpp
