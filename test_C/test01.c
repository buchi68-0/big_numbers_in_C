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
}

static void free_array(char **array)
{
    int i = 0;

    if (array == NULL)
        return;
    while (array[i] != NULL) {
        free(array[i]);
        i++;
    }
    free(array);
    return;
}

char *get_char_from_file(const char *filename)
{
    char *returned = (char *)malloc(sizeof(char) * 100000);
    FILE *f = fopen(filename, "r");
    int i;

    if (f == NULL || returned == NULL)
        return NULL;
    i = fread(returned, 1, 99999, f);
    returned[i] = '\0';
    fclose(f);
    printf("number in %s is of size %u\n", filename, i);
    return returned;
}

static void print_number_array(char **array)
{
    int i = 0;

    if (array == NULL)
        return;
    while (array[i] != NULL) {
        printf("number %d = %s\n\n", i + 1, array[i]);
        i++;
    }
}

static void set_null(char **array, int length)
{
    int i = 0;

    if (array == NULL)
        return;
    while (i < length) {
        array[i] = NULL;
        i++;
    }
    return;
}

int main(void)
{
    char *input1 = get_char_from_file("NUMBER1.mnb");
    char *input2 = get_char_from_file("NUMBER2.mnb");
    number_t a = parse_base10_str(input1);
    number_t b = parse_base10_str(input2);
    number_t c = sub_numbers(&a, &b);
    number_t d = add_numbers(&a, &b);
    number_t e = do_school_mul(&a, &b);
    char **array = (char **)malloc(12 * sizeof(char *));

    set_null(array, 12);
    array[0] = number_to_base10_str(&a);
    array[1] = number_to_base10_str(&b);
    array[2] = number_to_base10_str(&c);
    array[3] = number_to_base10_str(&d);
    array[4] = number_to_base10_str(&e);
    print_number_array(array);
    free_numbers(5, &a, &b, &c, &d, &e);
    free_array(array);
    free_strings(2, input1, input2);
    return 0;
}
