CC = c++
RM = rm -rf
XFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = server.cpp main.cpp client.cpp channel.cpp
OBJ = ${SRC:.cpp=.o}
NAME = ./ircserv
LIB  = server.hpp client.hpp channel.hpp

all : ${NAME}

${NAME} : ${OBJ} ${LIB}
		${CC} ${XFLAGS} ${OBJ} -o ${NAME}

%.o : %.cpp
	${CC} ${XFLAGS} -c $< -o $@

clean:
	${RM} ${OBJ}

fclean: clean
	${RM}  ${NAME}

re: fclean all

.PHONY: clean fclean re all