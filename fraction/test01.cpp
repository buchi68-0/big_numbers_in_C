/*
** EPITECH PROJECT, 2025
** test
** File description:
** is a main to see if alloc is good and all
*/

#include "main_dependencies.h"
#include <stdio.h>
#include <malloc.h>

int main(void)
{
    number_t a = parse_base10_str("12345678900000457498656841567485186521895618952169451149841456415611");
    char *printed;

    printed = number_to_base10_str(&a);
    printf("Yay ! The programm didn't crash !\n");
    printf("Here's a's pointer : %p\n", a.Value);
    printf("And there's it's first value : %u\n", a.Value[0]);
    printf("Now let's get to the string.\nIt's pointer is : %p\n", printed);
    printf("Here's a char from it : %c (%d)\n", printed[0], printed[0]);
    printf("and a is : %s\n", printed);
    my_free_number(&a);
    free(printed);
    return 0;
}
