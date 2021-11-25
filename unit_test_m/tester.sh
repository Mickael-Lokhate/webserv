#!/bin/bash

CONF_PATH="./confs/*"
INC_PATH="../inc"
SRCS_PATH="../srcs/"

Reset='\033[0m'       # Text Reset
Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'

echo -e "To redirect the output of each test add 1 argument\n"
echo -e "${Red}Compiling....${Reset}\n"
clang++ -Wall -Wextra -I$INC_PATH -g -fsanitize=address ${SRCS_PATH}Loader.cpp ${SRCS_PATH}Server.cpp ${SRCS_PATH}Route.cpp ${SRCS_PATH}LoaderTreatment.cpp ./main_test_loader.cpp

i=1
for f in $CONF_PATH
do
	echo -e "${Blue}Executing Test nº$i${Reset} - ${Red}FILE : $f${Reset}"
	if [ $# -eq 1 ] 
	then
		./a.out $f > test_output/test_${i}.out
	else
		./a.out $f 
	fi
	((i=i+1))
done
if [ $# -eq 1 ]
then
	echo -e "\n${Green}The output of each tests has been redirected in a file test_<test number>.out in the test_output folder${Reset}"
fi
rm -rf a.out a.out.dSYM
