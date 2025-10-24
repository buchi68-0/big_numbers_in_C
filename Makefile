##
## EPITECH PROJECT, 2025
## big int in C
## File description:
## compiles a basic programm
##

NAME = "test"

SRC = number_basic_operation.c	\
	number_conversion_procedures.c	\
	number_creation_procedures.c	\
	number_mul_procedures.c	\
	number_printable_procedure.c	\
	test01.c	\
	timespec_use.c

all : re

re : fclean
	clang -o $(NAME) $(SRC)

fclean : clean
	rm -f test

clean :
	rm -f *~
	rm -f \#*\#

debug : fclean
	epiclang -g -o $(NAME) $(SRC)

fsanit : fclean
	clang -fsanitize=address -g -o $(NAME) $(SRC)

gccver : fclean
	gcc -o $(NAME) $(SRC)
