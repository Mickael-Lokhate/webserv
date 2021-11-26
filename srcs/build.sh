#!/bin/sh
g++ -D DEBUG -Wall -Wextra -g -I ../inc Socket_client.cpp Socket_server.cpp Worker.cpp main.cpp
