/*
** EPITECH PROJECT, 2025
** big int
** File description:
** this file contains the function
** to convert a number_t back into char *
** it of course puts the - in case of negative
*/

#include "number_printable_dependencies.h"

static void fill_string(char *str, uint64_t char_len, number_t *copy)
{
    uint64_t nb_zeros = 0;
    uint64_t rest = 0;
    uint64_t pointer = 0;
    char contant[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    while (nb_zeros != copy->length) {
        for (uint64_t i = nb_zeros; i < copy->length; i++) {
            rest *= 4294967296;
            rest += copy->limbs[i];
            copy->limbs[i] = rest / 1000000000;
            nb_zeros += (i == nb_zeros && copy->limbs[i] == 0);
            rest = rest % 1000000000;
        }
        for (int j = 0; j < 9 && pointer < char_len; j++) {
            str[char_len - pointer - 1] = contant[rest % 10];
            rest /= 10;
            pointer++;
        }
        rest = 0;
    }
}

/* copies src into (*dest) while allocating (*dest) also */
static void copy_strings(char **dest, const char *src, uint64_t length)
{
    int i = 0;

    (*dest) = (char *)malloc((length + 1) * sizeof(char));
    if ((*dest) == NULL)
        return;
    while (i < length) {
        (*dest)[i] = src[i];
        i++;
    }
    (*dest)[i] = '\0';
}

/* handles trailing zeros from str
** case 1 : everything is zero : copy only one zero
** else : copies characters from the first non zero (!'0') character
** (+ 1 if negative, from one before to then put the sign) */
static char *handles_zero(char *str, uint64_t char_len, int sign)
{
    uint64_t nb_zero = 0;
    char *returned;

    while (str[nb_zero] == '0' && str[nb_zero] != '\0') {
        nb_zero++;
    }
    if (nb_zero == char_len) {
        copy_strings(&returned, str, 1);
        return returned;
    }
    if (sign) {
        str[nb_zero - 1] = '-';
        copy_strings(&returned, (str + nb_zero - 1), char_len - nb_zero + 1);
    } else {
        copy_strings(&returned, (str + nb_zero), char_len - nb_zero);
    }
    return returned;
}

/* memset with '0' (or 48) as the c byte
** puts a \0 at the end so length should be
** the length allocated - 1*/
static void fill_w_zeros(char *str, uint64_t len)
{
    uint64_t i = 0;

    while (i < len) {
        str[i] = '0';
        i++;
    }
    str[len] = '\0';
}

/* main function of the file
** converts Input into a string */
char *number_to_base10_str(number_t *Input)
{
    number_t copy = my_create_number(Input->length);
    uint64_t char_len = (Input->length * 9633) / 1000 + 50000;
    char *returned = (char *)malloc((char_len + 1) * sizeof(char));
    char *real_r;

    if (copy.limbs == NULL || returned == NULL) {
        my_free_number(&copy);
        free(returned);
    }
    my_memcpy(&copy, Input, 0);
    fill_w_zeros(returned, char_len);
    fill_string(returned, char_len, &copy);
    real_r = handles_zero(returned, char_len, Input->sign);
    my_free_number(&copy);
    free(returned);
    return real_r;
}
