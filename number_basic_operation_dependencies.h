/*
** EPITECH PROJECT, 2025
** Big ints
** File description:
** this file defines the prototypes of function
** used by Number_basic_operation_procedures
** to perform it's code
** uses the fuctions to create/allocate numbers
** (Number_creation_procedures.cpp)
*/

#ifndef BASIC_OP_DEPENDENCIES_
    #define BASIC_OP_DEPENDENCIES_
    #include "number_type.h"
    #define MIN(a, b) a < b ? a : b
    #define MAX(a, b) a > b ? a : b
number_t my_create_number(uint64_t base_length);
void my_free_number(number_t *Tofree);
void check_useless_zero(number_t *Number);
void my_memcpy(number_t *Dest, number_t *Src, int from);
#endif
