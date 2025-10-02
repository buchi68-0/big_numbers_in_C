/*
** EPITECH PROJECT, 2025
** big int
** File description:
** this file defines basics of number(s)
** and function used from other files
** (the file used is the one that
** defines the basics of creation
** of numbers)
*/

#ifndef PRINTABLE_DEPENDENCIES_
    #define PRINTABLE_DEPENDENCIES_
    #include "Number_type.h"
void my_free_number(number_t *Number);
number_t my_create_number(uint64_t base_length);
void my_memcpy(number_t *Dest, number_t *Src, int from);
#endif
