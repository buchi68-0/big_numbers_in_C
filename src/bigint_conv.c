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

    for (size_t i = number->len - 1; i < number->len; i--) {
        rest *= LIMB_BASE;
        rest += number->limb[i];
        number->limb[i] = rest / 1000000000ULL;
        rest %= 1000000000ULL;
    }
    return rest;
}

static void my_revstr(char *s)
{
    size_t length = strlen(s);
    char tmp;

    for (size_t i = 0; i < length / 2; i++) {
        tmp = s[i];
        s[i] = s[length - i - 1];
        s[length - i - 1] = tmp;
    }
}

static void track_last_zeros(char *s, size_t s_ptr, int8_t flag)
{
    while (s_ptr > 1 && s[s_ptr - 1] == '0') {
        s_ptr--;
    }
    if (flag) {
        s[s_ptr] = '-';
        s_ptr++;
    }
    s[s_ptr] = '\0';
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
        uint64_t division = 1;
        for (size_t i = 0; i < 9; i++, division *= 10) {
            s[s_ptr + i] = (result / division) % 10 + '0';
        }
        s_ptr += 9;
    } while (!is_number_zero(duplicate));
    free_number(duplicate);
    track_last_zeros(s, s_ptr, number->sign);
    my_revstr(s);
    return s;
}

bigint_t *convert_str_to_bigint(char const *s)
{
    
}