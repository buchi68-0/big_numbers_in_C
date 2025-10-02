/*
** EPITECH PROJECT, 2025
** Big numbers in C
** File description:
** this files allows basic creation of numbers,
** allowing them to be allocated, deallocated
*/

#include "Number_type.h"

//frees number's Value and nullifies it's caracteristics
void my_free_number(number_t *Tofree)
{
    if (Tofree->Value == NULL){
        return;
    }
    free(Tofree->Value);
    Tofree->Value = NULL;
    Tofree->length = 0;
}

//sets all values in ptr to 0
static void *set_number(uint32_t *ptr, uint64_t n)
{
    int i = 0;

    while (i < n) {
        ptr[i] = 0;
        i++;
    }
    return ptr;
}

//creates number of size base_length with all values set to 0
number_t my_create_number(uint64_t base_length)
{
    number_t r;

    r.Value = (uint32_t *)malloc(base_length * sizeof(uint32_t));
    set_number(r.Value, base_length);
    r.length = base_length;
    r.sign = 0;
    if (r.Value == NULL) {
        r.length = 0;
    }
    return r;
}

//copies Src content to Dest. Dest of size Src - from
void my_memcpy(number_t *Dest, number_t *Src, int from)
{
    uint64_t i = 0;

    if (Dest->Value == NULL) {
        Dest->length = 0;
        return;
    }
    while (i < Dest->length) {
        Dest->Value[i] = Src->Value[i + from];
        i++;
    }
    return;
}

void check_useless_zero(number_t *number)
{
    uint64_t nb_of_zero = 0;
    number_t temp;

    if (number == NULL || number->Value == NULL || number->length == 0)
        return;
    while (number->Value[nb_of_zero] == 0 && nb_of_zero < number->length)
        nb_of_zero++;
    if (nb_of_zero == number->length) {
        my_free_number(number);
        temp = my_create_number(1);
        *number = temp;
        return;
    }
    if (nb_of_zero != 0) {
        temp = my_create_number(number->length - nb_of_zero);
        my_memcpy(&temp, number, nb_of_zero);
        temp.sign = number->sign;
        my_free_number(number);
        *number = temp;
    }
}
