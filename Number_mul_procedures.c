/*
** EPITECH PROJECT, 2025
** big int in C
** File description:
** this file allows for multiplication
*/

#include "number_mul_dependencies.h"
#include <stdarg.h>

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

static number_t empty_number(void)
{
    number_t r;

    r.sign = 0;
    r.Value = NULL;
    r.length = 0;
    return r;
}

static void assign_nb(number_t *arg, number_t *array, uint64_t temp)
{
    if (temp > arg->length) {
        array->length = arg->length;
        array->sign = arg->sign;
        array->Value = arg->Value;
        (array + 1)->length = 0;
        (array + 1)->sign = 0;
        (array + 1)->Value = NULL;
        return;
    }
    array->length = temp;
    array->Value = arg->Value + arg->length - temp;
    array->sign = arg->sign;
    (array + 1)->Value = arg->Value;
    (array + 1)->sign = arg->sign;
    (array + 1)->length = arg->length - temp;
    return;
}

static void calculate_adds(number_t *dest, number_t *split)
{
    dest[0] = add_numbers(split, split + 1);
    dest[1] = add_numbers(split + 2, split + 3);
}

number_t bit_shift(number_t *from, uint64_t number)
{
    number_t r = my_create_number(from->length + number);

    if (r.Value == NULL)
        return empty_number();
    for (uint64_t i = 0; i < from->length; i++) {
        r.Value[i] = from->Value[i];
    }
    return r;
}

static void free_multiple(int number, ...)
{
    int i = 0;
    va_list ap;
    number_t *t;

    va_start(ap, number);
    while (i < number) {
        t = va_arg(ap, number_t *);
        my_free_number(t);
        i++;
    }
}

void calculate_muls(number_t *dst, number_t *res, number_t *to, uint64_t temp)
{
    free_multiple(2, res, res + 1);
    res[0] = sub_numbers(&dst[1], &dst[0]);
    res[1] = sub_numbers(res, &dst[2]);
    my_free_number(res);
    res[0] = bit_shift(&res[1], temp);
    (*to) = add_numbers(res, dst);
    free_multiple(4, res, res + 1, dst, dst + 1);
    if (dst[2].Value == NULL) {
        return;
    }
    res[1] = bit_shift(&dst[2], temp * 2);
    dst[0] = add_numbers(&res[1], to);
    free_multiple(3, dst + 2, res + 1, to);
    (*to) = dst[0];
    return;
}

static int is_zero(number_t *test)
{
    uint64_t i = 0;

    if (test == NULL || test->Value == NULL || test->length == 0) {
        return 1;
    }
    while (i < test->length && test->Value[i] == 0) {
        i++;
    }
    if (i >= test->length) {
        return 1;
    }
    return 0;
}

number_t mul_numbers(number_t *arg1, number_t *arg2)
{
    uint64_t temp = ((MAX(arg1->length, arg2->length)) + 1) / 2;
    number_t r;
    number_t split[4];
    number_t results[2];
    number_t mul_res[3] = {empty_number(), empty_number(), empty_number()};

    if (is_zero(arg1) || is_zero(arg2))
        return empty_number();
    if (arg1->length < 30 && arg2->length < 30)
        return do_school_mul(arg1, arg2);
    assign_nb(arg1, split, temp);
    assign_nb(arg2, split + 2, temp);
    calculate_adds(results, split);
    mul_res[0] = mul_numbers(split, split + 2);
    mul_res[1] = mul_numbers(results, results + 1);
    if (split[1].length != 0 && split[3].length != 0)
        mul_res[2] = mul_numbers(split + 1, split + 3);
    calculate_muls(mul_res, results, &r, temp);
    check_useless_zero(&r);
    return r;
}
