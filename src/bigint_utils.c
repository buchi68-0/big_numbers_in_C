/*
** EPITECH PROJECT, 2026
** bigint
** File description:
** bigint_utils.c
** utils for bigints such as duplication, creation or free
*/

#include "bigint.h"
#include <malloc.h>
#include <string.h>

bigint_t *duplicate_number(bigint_t const *number)
{
    bigint_t *new_number = malloc(sizeof(bigint_t));

    if (!new_number)
        return NULL;
    new_number->len = number->len;
    new_number->sign = number->sign;
    new_number->limb = malloc(new_number->len * sizeof(*(number->limb)));
    if (!new_number->limb) {
        free(new_number);
        return NULL;
    }
    memcpy(new_number->limb, number->limb, new_number->len * sizeof(*(number->limb)));
    return new_number;
}

void free_number(bigint_t *number)
{
    if (!number)
        return;
    free(number->limb);
    memset(number, 0, sizeof(bigint_t)); // protection for invalid reads after free
    free(number);
}

bigint_t *create_number(size_t len, int8_t flag)
{
    bigint_t *new_number = malloc(sizeof(bigint_t));

    if (!new_number)
        return NULL;
    new_number->len = len;
    new_number->sign = flag;
    new_number->limb = calloc(len, sizeof(*(new_number->limb)));
    if (!new_number->limb) {
        free(new_number);
        return NULL;
    }
    return new_number;
}

int8_t is_number_zero(bigint_t *number)
{
    if (!number)
        return 1;
    for (size_t i = 0; i < number->len; i++) {
        if (number->limb[i] != 0)
            return 0;
    }
    return 1;
}
