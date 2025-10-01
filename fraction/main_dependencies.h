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
void my_free_number(number_t *Tofree);
number_t my_create_number(uint64_t base_length);
void check_useless_zero(number_t *Number);
number_t parse_base10_str(const char *string);
char *number_to_base10_str(number_t *Number);
#endif
