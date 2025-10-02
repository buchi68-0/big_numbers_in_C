/*
** EPITECH PROJECT, 2025
** test
** File description:
** is a main to see if alloc is good and all
*/

#include "main_dependencies.h"
#include <stdio.h>
#include <stdarg.h>

static void free_numbers(int number, ...)
{
    int i = 0;
    va_list ap;
    number_t *to_free;

    va_start(ap, number);
    while (i < number) {
        to_free = va_arg(ap, number_t *);
        my_free_number(to_free);
        i++;
    }
    return;
}

static void free_strings(int number, ...)
{
    int i = 0;
    va_list ap;
    char *to_free;

    va_start(ap, number);
    while (i < number) {
        to_free = va_arg(ap, char *);
        free(to_free);
        i++;
    }
    return;
}

char *get_char_from_file(const char *filename)
{
    char *returned = (char *)malloc(sizeof(char) * 1000000);
    FILE *f = fopen(filename, "r");
    int i;

    if (f == NULL || returned == NULL)
        return NULL;
    i = fread(returned, 1, 999999, f);
    returned[i] = '\0';
    printf("number in %s is of size %u", filename, i);
    return returned;
}

int main(void)
{
    char *input1 = get_char_from_file("NUMBER1.mnb");
    char *input2 = get_char_from_file("NUMBER2.mnb");
    printf("ptr : %p & %p\n\n", input1, input2);
    printf("input1 : %s\n\n", input1);
    printf("input2 : %s\n\n", input2);
    number_t a = parse_base10_str(input1);
    number_t b = parse_base10_str(input2);
    number_t c = sub_numbers(&a, &b);
    number_t d = add_numbers(&a, &b);
    char *printed;
    char *printed2;
    char *test;
    char *test2;

    printed = number_to_base10_str(&a);
    printed2 = number_to_base10_str(&b);
    test = number_to_base10_str(&c);
    test2 = number_to_base10_str(&d);
    printf("Yay ! The programm didn't crash !\n");
    printf("Here's a's pointer : %p\n", a.Value);
    printf("Now let's get to the string.\nIt's pointer is : %p\n", printed);
    printf("and a is : %s\n", printed);
    printf("and    b : %s\n", printed2);
    printf("and    c : %s\n", test);
    printf("and    d : %s\n", test2);
    free_numbers(4, &a, &b, &c, &d);
    free_strings(6, printed, printed2, test, test2, input1, input2);
    return 0;
}
