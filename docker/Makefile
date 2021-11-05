GREEN = "\033[32m"
WHITE = "\033[0m"

define print
	echo -e $(GREEN)$(1)$(WHITE)
endef

DATA_DIR = ./data

_C = docker ps -aq
_I = docker images -aq
_V = docker volume ls -q
_N = docker network ls -q
 
NB_C = $$($(_C) | wc -l)
NB_I = $$($(_I) | wc -l)
NB_V = $$($(_V) | wc -l)
NB_N = $$($(_N) | wc -l)

clean		: SHELL:=/bin/bash
fclean		: SHELL:=/bin/bash
all			: SHELL:=/bin/bash
up			: SHELL:=/bin/bash
build		: SHELL:=/bin/bash
logclean	: SHELL:=/bin/bash

all     : clean logclean up


up     	:
	@$(call print,"- Docker-compose Up")
	@docker-compose -p web-serv up -d 

build   : 
	@$(call print,"- Docker-compose build")
	@docker-compose build

clean   : 
	@$(call print,"- Clean containers")
	@[ $(NB_C) -ne 0 ] && docker rm -f $$($(_C)); true
	@$(call print,"- Clean volumes")
	@[ $(NB_V) -ne 0 ] && docker volume rm -f $$($(_V)); true
	@$(call print,"- Clean networks")
	@[ $(NB_N) -ne 0 ] && docker network rm $$($(_N)) 2> /dev/null ; true

fclean	: clean
	@$(call print,"- Clean images")
	@[ $(NB_I) -ne 0 ] && docker rmi -f $$($(_I)); true

logclean :
	@$(call print,"- Clean logs")
	@rm -rf log/nginx/* ; true


re	: fclean all

.PHONY: all clean fclean re
