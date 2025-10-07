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

#include "number_conv_dependencies.h"

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

static int is_invalid_string(char *string, int64_t *ptr)
{
    int i = 0;

    while (string[i] != '\0') {
        if (string[i] > '9' || string[i] < '0') {
            string[i] = '\0';
            *ptr = i;
            return 0;
        }
        i++;
    }
    *(ptr) = i;
    return 0;
}

static void handle_sign(int8_t *sign, char *string)
{
    if (string == NULL || string[0] == '\0') {
        (*sign) = 0;
        return;
    }
    if (string[0] == '-') {
        (*sign) = 1;
        string[0] = '0';
    }
    (*sign) = 0;
    return;
}

uint64_t divide_str(uint64_t *input, uint64_t *number_of_zeros, int64_t length)
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

static number_t empty_number(void)
{
    number_t r;

    r.Value = NULL;
    r.sign = 0;
    r.length = 0;
    return r;
}

static number_t define_number(char *string, int64_t *string_length)
{
    number_t r;
    int8_t sign;

    handle_sign(&sign, string);
    is_invalid_string(string, string_length);
    r = my_create_number((*(string_length) * 1000) / 9633 + 2);
    r.sign = sign;
    if (string_length <= 0 || r.Value == NULL)
        return empty_number();
    return r;
}

//divides string over and over again until it's all 0's
number_t parse_base10_str(char *string)
{
    int64_t string_length;
    number_t r = define_number(string, &string_length);
    uint64_t *new_input = compress_str(string, &string_length, string_length);
    uint64_t division_h[3] = {0, 1, 0};

    if (r.Value == NULL || new_input == NULL)
        return r;
    while (division_h[2] < string_length) {
        division_h[0] = divide_str(new_input, division_h + 2, string_length);
        r.Value[r.length - division_h[1]] = division_h[0];
        division_h[1]++;
    }
    check_useless_zero(&r);
    free(new_input);
    return r;
}
