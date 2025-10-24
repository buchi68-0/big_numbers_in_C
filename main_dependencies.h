/*
** EPITECH PROJECT, 2025
** big ints
** File description:
** this file is a failsafe
** to prevent from implicit declaration
*/

#ifndef MAIN_NUMBER_FAILSAFE_
    #define MAIN_NUMBER_FAILSAFE_
    #include "Number_type.h"
    #define MAX_STR 200000
    #include <time.h>
void my_free_number(number_t *Tofree);
number_t my_create_number(uint64_t base_length);
void check_useless_zero(number_t *Number);
number_t parse_base10_str(const char *string);
char *number_to_base10_str(number_t *Number);
number_t sub_numbers(number_t *n1, number_t *n2);
number_t add_numbers(number_t *n1, number_t *n2);
number_t mul_numbers(number_t *n1, number_t *n2);
number_t bit_shift(number_t *n1, uint64_t temp);
typedef struct timespec timespec_t;
void timespec_treat(timespec_t *dst, const timespec_t *src);
#endif
