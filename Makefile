NAME = webserv

SRCS = main.cpp ServerSocket.cpp ServerConfig.cpp \
	testConfigParser.cpp makeQueue.cpp Request.cpp \
	socketIO.cpp


OBJS = $(SRCS:.cpp=.o)

DEPS = $(SRCS:.cpp=.d)

FLAGS = -g -Wall -Werror -Wextra -std=c++98

CC = clang++

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $^ -o $@

%.o: %.cpp Makefile
	$(CC) $(FLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all
