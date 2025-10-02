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

int main(void)
{
    number_t a = parse_base10_str("123456789000012356565485164654985415669535297329239252592372372727232525657217627257611262112762712611212127117267212162672456259416975693285256382329464294323422121329213312121231212342567529757685722986532495735625575792432579523283823852372495467375626973253222769276276227327237492572537267986579559757936267253723795378579686231329732756756575765957657565657566792312397687323264542261213579656324297567532613297383622131657594216237565675676756753646297462462267765272576269779525597257");
    number_t b = parse_base10_str("-2345678900001235656548516465498541566953529732923925259237237272723");
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
    free_strings(4, printed, printed2, test, test2);
    return 0;
}
