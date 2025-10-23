/*
** EPITECH PROJECT, 2025
** test
** File description:
** is a main to see if alloc is good and all
*/

#include "main_dependencies.h"
#include <stdio.h>
#include <stdarg.h>

/* using a variadic list, frees
** up to number number_t* variables
** the number of args in the ... are
** expected to be at least the value of number
** all values in the va_list are expected to be
** number_t *.
** other values will cause seg-faults */
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

/* same as free_array (see below)
** except it uses a variadic list
** to free multiple strings.
** the number of args in ... are
** expected to be at least the value of number
** all values in the va_list are expected to be char *
** other values could cause seg-faults */
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

// frees all strings in array. stops at first NULL
// then frees array itself
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

/* returns char * from file in 'filename'
** file is opened using stdio.h's fopen
** prints size of the ACTUAL size of returned
** (which is MIN(MAX_STR, FILE_SIZE)) */
char *get_char_from_file(const char *filename)
{
    char *returned = (char *)malloc(sizeof(char) * (MAX_STR + 1));
    FILE *f = fopen(filename, "r");
    int i;

    if (f == NULL || returned == NULL)
        return NULL;
    i = fread(returned, 1, MAX_STR, f);
    returned[i] = '\0';
    fclose(f);
    printf("number in %s is of size %u\n", filename, i);
    return returned;
}

/* this function prints all numbers in the array.
** mode sets some conditions :
** 0 : no printing, for no thick layers of the same digit for
** thousands of kilobytes
** 1 : prints limited, doesn't print the 2 numbers
** that were in the file.
** 2 : prints all */
static void print_number_array(char **array, int mode)
{
    int i = 0;

    if (mode == 0)
        return;
    if (mode == 1)
        i = 2;
    if (array == NULL)
        return;
    while (array[i] != NULL) {
        printf("number %d = %s\n\n", i + 1, array[i]);
        i++;
    }
}

/* this function sets null in the array so all
** values are NULL and can be easily found as
** "terminating bytes" (pointers then) */
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

/* fills the array with number number_t *values
** uses a variadic list, and of course it expects
** at least the same amount of args after the ...
** than the value of number
** all values are also expected to be number_t *
** other values will for sure cause seg-faults */
static void fill_array(char **array, int number, ...)
{
    int i = 0;
    va_list ap;
    number_t *get;

    va_start(ap, number);
    while (i < number) {
        get = va_arg(ap, number_t *);
        array[i] = number_to_base10_str(get);
        i++;
    }
    return;
}

/* sample of tests.
** NUMBER1.mnb and NUMBER2.mnb are already filled
** so the test can be compiled and used 'as is'
** files can be changed without having to recompile
** this version of the test has the mode 0 in
** print_number_array, so it doesn't display anything
** it works, though
** max size of string can be changed in
** the main_dependencies.h */
int main(void)
{
    char *input1 = get_char_from_file("NUMBER1.mnb");
    char *input2 = get_char_from_file("NUMBER2.mnb");
    number_t *n_array = malloc(12 * sizeof(number_t));
    number_t a = parse_base10_str(input1);
    number_t b = parse_base10_str(input2);
    number_t c = sub_numbers(&a, &b);
    number_t d = add_numbers(&a, &b);
    number_t e = mul_numbers(&a, &b);
    char **array = (char **)malloc(12 * sizeof(char *));

    set_null(array, 12);
    fill_array(array, 0, &e);
    print_number_array(array, 0);
    free_numbers(5, &a, &b, &c, &d, &e);
    free_array(array);
    free_strings(2, input1, input2);
    return 0;
}
