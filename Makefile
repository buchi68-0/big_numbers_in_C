##
## EPITECH PROJECT, 2025
## big int in C
## File description:
## compiles a basic programm
##

NAME = "test"

SRC = Number_basic_operation.c	\
	Number_conversion_procedures.c	\
	Number_creation_procedures.c	\
	Number_mul_procedures.c	\
	Number_printable_procedure.c	\
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
