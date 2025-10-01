/*
** EPITECH PROJECT, 2025
** Big ints
** File description:
** allows for basic number_t conversion,
** from base10 to base2 (char *)->(number_t)
** string verification is stubborn
** only one sign allowed
** and only digits after that
** if errors, return empty number
*/

#include "Number_type.h"
#include "number_conv_dependencies.h"
#include <malloc.h>
#include <stddef.h>

static int64_t my_strlen(const char *str)
{
    int i = 0;

    if (str == NULL) {
        return -1;
    }
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

static void set_object_zero(uint64_t *ptr, uint64_t length)
{
    int i = 0;

    while (i < length) {
        ptr[i] = 0;
        i++;
    }
}

uint64_t *compress_str(const char *Input, int64_t *len_h, int64_t ip_len)
{
    uint64_t r_len = ip_len / 9 + 1;
    uint64_t *r = (uint64_t *)malloc(r_len * sizeof(uint64_t));
    uint64_t pointer = 1;

    set_object_zero(r, r_len);
    for (int i = 0; i < ip_len % 9; i++) {
        r[0] *= 10;
        r[0] += Input[i] - '0';
    }
    for (int i = ip_len % 9; i < ip_len; i += 9) {
        for (int j = 0; j < 9; j++) {
            r[pointer] *= 10;
            r[pointer] += Input[i + j] - '0';
        }
        pointer++;
    }
    *(len_h) = r_len;
    return r;
}

static int is_invalid_string(const char *string)
{
    int i = 0;

    while (string[i] != '\0') {
        if (string[i] > '9' || string[i] < '0') {
            return 1;
        }
        i++;
    }
    return 0;
}

static void handle_sign(number_t *r, int64_t *input_len, const char **string)
{
    if ((*string)[0] == '-') {
        r->sign = 1;
        *input_len = *input_len - 1;
        *string = *string + 1;
    }
    return;
}

uint64_t divide_str(uint64_t *input, int64_t *number_of_zeros, int64_t length)
{
    uint64_t division = 0;
    uint64_t rest = 0;

    for (int64_t i = (*number_of_zeros); i < length; i++) {
        rest *= 1000000000;
        rest += input[i];
        division = rest / 4294967296;
        rest %= 4294967296;
        input[i] = division;
        if (i == (*number_of_zeros) && input[i] == 0) {
            (*number_of_zeros)++;
        }
    }
    return rest;
}

//divides string over and over again until it's all 0's
number_t parse_base10_str(const char *string)
{
    int64_t string_length = my_strlen(string);
    number_t r = my_create_number((string_length * 1000) / 9633 + 2);
    uint64_t *new_input;
    int64_t nb_zero = 0;
    uint64_t division_h[2] = {0, 1};

    if (string_length <= 0 || r.Value == NULL)
        return {NULL, 0, 0};
    handle_sign(&r, &string_length, &string);
    if (string_length <= 0 || is_invalid_string(string))
        return {NULL, 0, 0};
    new_input = compress_str(string, &string_length, string_length);
    while (nb_zero < string_length) {
        division_h[0] = divide_str(new_input, &nb_zero, string_length);
        r.Value[r.length - division_h[1]] = division_h[0];
        division_h[1]++;
    }
    check_useless_zero(&r);
    free(new_input);
    return r;
}
