/*
** EPITECH PROJECT, 2025
** big int in C
** File description:
** this file allows for multiplication
*/

#include "number_mul_dependencies.h"

//if n2->length > -> unexpected behaviour ; if n1->length > -> crash
static void private_add_assign(number_t *n1, number_t *n2)
{
    uint64_t number = 0;

    for (uint64_t i = n1->length - 1; i < n1->length; i--) {
        number += n2->Value[i];
        number += n1->Value[i];
        n1->Value[i] = number;
        number = number >> 32;
        n2->Value[i] = 0;
    }
}

number_t do_school_mul(number_t *multiplied, number_t *multiplier)
{
    number_t result = my_create_number(multiplied->length + multiplier->length);
    number_t temp = my_create_number(result.length);
    uint64_t temporary = 0;
    int8_t flag = 0;

    for (uint64_t k = 0; k < multiplied->length; k++) {
        for (uint64_t j = 0; j < multiplier->length; j++) {
            temporary = multiplied->Value[multiplied->length - k - 1];
            temporary *= multiplier->Value[multiplier->length - j - 1];
            temp.Value[temp.length - k - j - 1] += temporary;
            flag = (temp.Value[temp.length - k - j - 1] < (temporary & MAXINT));
            temp.Value[temp.length - k - j - 2] += flag;
            temp.Value[temp.length - k - j - 2] += temporary >> 32;
        }
        private_add_assign(&result, &temp);
    }
    my_free_number(&temp);
    result.sign = multiplied->sign ^ multiplier->sign;
    check_useless_zero(&result);
    return result;
}
