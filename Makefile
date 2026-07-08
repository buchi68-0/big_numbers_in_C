##
## PERSONNAL PROJECT, 2026
## bigint
## File description:
## Compiles the lib and potentially compiles the test
##

SRC =	src/operations.c	\
		src/bigint_conv.c	\
		src/bigint_utils.c	\

CFLAGS += -I./include

NAME = libbigint.a

TEST_NAME = test.out

OBJ = $(SRC:.c=.o)

all : $(OBJ)
	ar rc $(NAME) $(OBJ)

re : fclean all

$(NAME) : all

fclean : clean
	rm -f $(NAME)
	rm -f $(TEST_NAME)

clean:
	rm -f $(OBJ)

test: all
	$(CC) $(CFLAGS) -o $(TEST_NAME) main.c -L. -lbigint

opti: CFLAGS += -O3
opti: re

optest: opti test

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@