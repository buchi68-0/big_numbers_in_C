/*
** EPITECH PROJECT, 2025
** Big numbers in C
** File description:
** this files allows basic creation of numbers,
** allowing them to be allocated, deallocated
*/

#include "number_type.h"

//frees number's limbs and nullifies it's caracteristics
void my_free_number(number_t *Tofree)
{
    if (Tofree->limbs == NULL){
        return;
    }
    free(Tofree->limbs);
    Tofree->limbs = NULL;
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
number_t my_create_number(int64_t base_length)
{
    number_t r;

    if (base_length < 0) {
        r.sign = 0;
        r.limbs = NULL;
        r.length = 0;
        return r;
    }
    r.limbs = (uint32_t *)malloc(base_length * sizeof(uint32_t));
    set_number(r.limbs, base_length);
    r.length = base_length;
    r.sign = 0;
    if (r.limbs == NULL) {
        r.length = 0;
    }
    return r;
}

//copies Src content to Dest. Dest of size Src - from
void my_memcpy(number_t *Dest, number_t *Src, int from)
{
    uint64_t i = 0;

    if (Dest->limbs == NULL) {
        Dest->length = 0;
        return;
    }
    while (i < Dest->length) {
        Dest->limbs[i] = Src->limbs[i + from];
        i++;
    }
    return;
}

//reallocs number into smaller size with no trailing zeros
void check_useless_zero(number_t *number)
{
    uint64_t nb_of_zero = 0;
    number_t temp;

    if (number == NULL || number->limbs == NULL || number->length == 0)
        return;
    while (number->limbs[nb_of_zero] == 0 && nb_of_zero < number->length - 1)
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
