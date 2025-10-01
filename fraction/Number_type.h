/*
** EPITECH PROJECT, 2025
** Big Number in X
** File description:
** Defines the Number type
** requires stdint.h for uint32_t (for example)
*/

#ifndef _BIGINT_NUMBER_STRUCT
    #define _BIGINT_NUMBER_STRUCT
    #include <stdint.h>

typedef struct number_s {
    uint32_t *Value;
    uint64_t length;
    int sign;
} number_t;
#endif
