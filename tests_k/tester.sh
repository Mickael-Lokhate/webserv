#!/bin/bash

BLACK='\033[0;30m'
RED='\033[0;31m'
GREEN='\033[0;32m'
BROWN='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
LGREY='\033[0;37m'
DGREY='\033[1;30m'
LRED='\033[1;31m'
LGREEN='\033[1;32m'
YELLOW='\033[1;33m'
LBLUE='\033[1;34m'
LPURPLE='\033[1;35m'
RESET='\033[0m'

INC_PATH="../inc"
SRCS_PATH="../srcs/"
EXEC="test"

echo -e "${GREEN}Starting tester...${RESET}\n"
clang++ -Wall -Wextra -I$INC_PATH -g -fsanitize=address test.cpp ${SRCS_PATH}Server.cpp ${SRCS_PATH}Route.cpp ${SRCS_PATH}utils.cpp -o ${EXEC}

if test -f "./${EXEC}"
then
		./${EXEC} > log_out
		diff log_out log_goal > diff.txt
		if [ -s diff.txt ]
		then
			echo -e "${RED}Differences detected !${RESET}\n"
		else
			echo -e "${GREEN}Expected output !${RESET}\n"
		fi
else
	echo -e "\n${RED}ERROR: Compilation failed${RESET}\n"
fi