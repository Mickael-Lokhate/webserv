NAME = TesterServer

CXX = clang++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g -I ../inc

SRCS = test.cpp TesterServer.cpp ../srcs/Server.cpp \
		../srcs/Route.cpp ../srcs/utils.cpp
OBJS = $(SRCS:.cpp=.o)

ifdef DEBUG
	CXXFLAGS += -D DEBUG
endif

all: $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean :
	rm -rf $(SRCS:.cpp=.o)

fclean : clean
	rm -rf $(NAME)

re : fclean all

run :
	make all
	./$(NAME)

reD : fclean
	make DEBUG=1 re
	./$(NAME)

.PHONY: all re fclean clean run reD