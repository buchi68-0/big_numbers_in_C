/*
** EPITECH PROJECT, 2025
** bit int
** File description:
** this is a failsafe
** for Number conversion procedures
*/

#ifndef N_CONV_DEPENDENCIES_
    #define N_CONV_DEPENDENCIES_
    #include "Number_type.h"
number_t my_create_number(uint64_t base_length);
void check_useless_zero(number_t *Number);
void my_free_number(number_t *number);
#endif
