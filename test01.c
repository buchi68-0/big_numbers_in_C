/*
** EPITECH PROJECT, 2025
** test
** File description:
** is a main to see if alloc is good and all
*/

#include "main_dependencies.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/* frees all numbers in array up to number
** number has to be at most the length of array
** array must be filled up to number
*/
static void free_number_array(int number, number_t *array)
{
    int i = 0;

    while (i < number) {
        my_free_number(array + i);
        i++;
    }
    free(array);
}

/* same as free_array (see below)
** except it uses a variadic list
** to free multiple heap memory.
** the number of args in ... are
** expected to be at least the value of number
** all values in the va_list are expected to be void *
** all void * are expected to be valid pointers in heap
** other values could cause seg-faults */
static void free_memory(int number, ...)
{
    int i = 0;
    va_list ap;
    char *to_free;

    va_start(ap, number);
    while (i < number) {
        to_free = va_arg(ap, void *);
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

/* fills the array with number number_t *values
** uses a variadic list, and of course it expects
** at least the same amount of args after the ...
** than the value of number
** all values are also expected to be number_t *
** other values will for sure cause seg-faults */
static timespec_t *fill_array(char **array, int number, ...)
{
    int i = 0;
    va_list ap;
    number_t *get;
    timespec_t r;
    timespec_t *empty = calloc(1, sizeof(timespec_t));

    if (!timespec_get(&r, TIME_UTC) || empty == NULL)
        return NULL;
    va_start(ap, number);
    while (i < number) {
        get = va_arg(ap, number_t *);
        array[i] = number_to_base10_str(get);
        i++;
    }
    if (!timespec_get(empty, TIME_UTC))
        return empty;
    timespec_treat(empty, &r);
    return empty;
}

static timespec_t *parse(number_t *array, char *input1, char *input2)
{
    timespec_t r;
    timespec_t *empty = calloc(1, sizeof(timespec_t));

    if (!timespec_get(&r, TIME_UTC) || empty == NULL)
        return NULL;
    array[0] = parse_base10_str(input1);
    array[1] = parse_base10_str(input2);
    if (!timespec_get(empty, TIME_UTC))
        return empty;
    timespec_treat(empty, &r);
    return empty;
}

static timespec_t *calculate(number_t *array)
{
    timespec_t r;
    timespec_t *returned = calloc(1, sizeof(timespec_t));

    if (!timespec_get(&r, TIME_UTC) || returned == NULL)
        return NULL;
    array[2] = add_numbers(array, array + 1);
    array[3] = sub_numbers(array, array + 1);
    array[4] = mul_numbers(array, array + 1);
    if (!timespec_get(returned, TIME_UTC))
        return returned;
    timespec_treat(returned, &r);
    return returned;
}

/* sample of tests.
** NUMBER1.mnb and NUMBER2.mnb are already filled
** so the test can be compiled and used 'as is'
** files can be changed without having to recompile
** this version of the test has the mode 0 in
** print_number_array, so it doesn't display anything
** it works, though
** max size of string can be changed in
** the main_dependencies.h
** if you want to see a display of the numbers
** just add said numbers' pointer into fill_array
** change the int in fill_array to fit the number
** of number_t * and then add these numbers into
** print_number_array (and change the mode to 2)*/
int main(void)
{
    char *input1 = get_char_from_file("NUMBER1.mnb");
    char *input2 = get_char_from_file("NUMBER2.mnb");
    number_t *n_array = calloc(12, sizeof(number_t));
    timespec_t *first = parse(n_array, input1, input2);
    timespec_t *second = calculate(n_array);
    timespec_t *third;
    char **array = (char **)calloc(12, sizeof(char *));

    third = fill_array(array, 0, n_array);
    print_number_array(array, 0);
    printf("parsing : %ld.%.9lds\n", first->tv_sec, first->tv_nsec);
    printf("operations : %ld.%.9lds\n", second->tv_sec, second->tv_nsec);
    printf("conversion : %ld.%.9lds\n", third->tv_sec, third->tv_nsec);
    free_array(array);
    free_number_array(5, n_array);
    free_memory(5, input1, input2, first, second, third);
    return 0;
}
