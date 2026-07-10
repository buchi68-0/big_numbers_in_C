##
## PERSONNAL PROJECT, 2026
## bigint
## File description:
## Compiles the lib and potentially compiles the test
##

SRC =	src/operations.c				\
		src/bigint_conv.c				\
		src/bigint_utils.c				\
		src/fast_fourier_transform.c	\

CFLAGS += -I./include

NAME = libbigint.a

TEST_NAME = test.out

LIBS = 	-lbigint	\
		-lm			\
		

OBJ = $(SRC:.c=.o)

all : $(OBJ)
	ar rc $(NAME) $(OBJ) -lm

re : fclean all

$(NAME) : all

fclean : clean
	rm -f $(NAME)
	rm -f $(TEST_NAME)

clean:
	rm -f $(OBJ)

test: all
	$(CC) $(CFLAGS) -o $(TEST_NAME) main.c -L. $(LIBS)

opti: CFLAGS += -O3
opti: re

optest: opti test

debug: CFLAGS += -g
debug: re

testbug: debug test

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@