/*
** EPITECH PROJECT, 2026
** bigint operations
** File description:
** bigint_conv.c
** does the conversions of a big int
*/

#include "bigint.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>

static size_t approximate_bigint_length(bigint_t const *number)
{
    size_t length = 0;

    if (!number || !number->limb)
        return 0;
    length = (number->len * 9633) / 1000;
    return length + 10;
}

static uint64_t divide_number_by_one_billion(bigint_t *number)
{
    uint64_t rest = 0;
    int8_t found_non_zero = 0;

    for (ssize_t i = number->len - 1; i >= 0; i--) {
        rest <<= LIMB_BITS;
        rest += (uint64_t)number->limb[i];
        number->limb[i] = rest / 1000000000ULL;
        rest %= 1000000000ULL;
        if (!found_non_zero) {
            if (!number->limb[i]) {
                number->len--;
            } else {
                found_non_zero = 1;
            }
        }
    }
    return rest;
}

static void my_revstr(char *s, size_t s_ptr)
{
    char tmp;

    for (size_t i = 0; i < s_ptr / 2; i++) {
        tmp = s[i];
        s[i] = s[s_ptr - i - 1];
        s[s_ptr - i - 1] = tmp;
    }
}

char *convert_bigint_to_str(bigint_t const *number)
{
    char *s;
    bigint_t *duplicate;
    uint64_t result;
    size_t s_len;
    size_t s_ptr = 0;

    if (!number || !number->limb)
        return NULL;
    s_len = approximate_bigint_length(number);
    s = malloc(s_len * sizeof(char));
    if (!s)
        return NULL;
    duplicate = duplicate_number(number);
    do
    {
        uint64_t result = divide_number_by_one_billion(duplicate);
        if (is_number_zero(duplicate)) {
            do {
                s[s_ptr++] = (result % 10) + '0';
                result /= 10;
            } while (result);
            if (number->sign) {
                s[s_ptr++] = '-';
            }
            s[s_ptr] = '\0';
            break;
        } else {
            for (int i = 0; i < 9; i++) {
                s[s_ptr++] = (result % 10) + '0';
                result /= 10;
            }
        }
    } while (1);
    free_number(duplicate);
    my_revstr(s, s_ptr);
    return s;
}

static uint32_t *compress_str(char const *s, size_t *returned_len_holder, size_t len)
{
    size_t r_len = (len - 1) / 9 + 1;
    uint32_t *returned = calloc(r_len, sizeof(uint32_t));
    size_t pointer = 0;

    for (ssize_t i = len - 1; i > 8; i -= 9) {
        for (int j = 0; j < 9; j++) {
            returned[pointer] *= 10;
            returned[pointer] += s[i - 8 + j] - '0';
        }
        pointer++;
    }
    for (size_t i = 0; i < len % 9; i++) {
        returned[pointer] *= 10;
        returned[pointer] += s[i] - '0';
    }
    (*returned_len_holder) = r_len;
    return returned;
}

static size_t check_string(char const *s, int8_t *sign_holder, char const **s_holder)
{
    size_t i = 0;

    if (!s)
        return 0;
    while (s[i] == '-' || s[i] == '+') {
        if (s[i] == '-')
            (*sign_holder)++;
        i++;
    }
    if (!s[i])
        return 0;
    i--;
    while (s[i + 1] == '0') {
        i++;
    }
    if (s[i + 1] < '0' || s[i + 1] > '9') { // just a 0
        return 1;
    }
    i++;
    (*s_holder) = s + i;
    return strspn(s + i, "0123456789");
}

static uint32_t divide_string_by_2_to_the_32(uint32_t *restrict compressed, size_t *restrict compressed_len_holder)
{
    uint64_t rest = 0;
    int8_t found_a_non_zero = 0;

    for (ssize_t i = (*compressed_len_holder) - 1; i >= 0; i--) {
        rest *= 1000000000UL;
        rest += (uint64_t)compressed[i];

        compressed[i] = (uint32_t)(rest >> LIMB_BITS);
        if (!found_a_non_zero) {
            if (!compressed[i]) {
                (*compressed_len_holder)--;
            } else {
                found_a_non_zero = 1;
            }
        }
        rest &= LIMB_MASK;
    }

    return (uint32_t)rest;
}

bigint_t *convert_str_to_bigint(char const *s)
{
    int8_t sign = 0;
    size_t len = check_string(s, &sign, &s);

    if (len == 0) {
        return NULL;
    }
    bigint_t *new_number = create_number((len * 1000) / 9633 + 10, sign % 2);
    if (!new_number) {
        return NULL;
    }
    size_t holder = 0;
    uint32_t *compressed = compress_str(s, &holder, len);
    size_t ptr = 0;

    if (!compressed) {
        free_number(new_number);
        return NULL;
    }

    while (holder != 0) {
        new_number->limb[ptr++] = divide_string_by_2_to_the_32(compressed, &holder);
    }

    free(compressed);
    return new_number;
}
