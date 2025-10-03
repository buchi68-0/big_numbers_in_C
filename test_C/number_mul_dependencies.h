/*
** EPITECH PROJECT, 2025
** big int in c
** File description:
** this file defines prototypes of the functions
** that'll be used by Number_mul_procedures.c
*/

#ifndef MULTIPLICATION_HEADER_H
    #define MULTIPLICATION_HEADER_H
    #include "Number_type.h"
    #define MAXINT 4294967295
number_t my_create_number(uint64_t base_length);
void my_free_number(number_t *Tofree);
void check_useless_zero(number_t *Number);
void my_memcpy(number_t *Dest, number_t *Src, int from);
#endif
