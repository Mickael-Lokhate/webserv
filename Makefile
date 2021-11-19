NAME		= webserv
SRC			= 
OBJ			= $(SRC:.cpp=.o)
FLAGS		= -g -Wall -Wextra -Werror -std=c++98

all			: $(NAME)

$(NAME)		: $(OBJ) 
			g++ $(FLAGS) -o $@ $^

%.o			: %.cpp
			g++ $(FLAGS) -o $@ -c $^

clean		:
			rm -rf *.o

fclean		: clean
			rm -rf $(NAME) test

re			: fclean all
