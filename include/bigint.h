/*
** PERSONNAL PROJECT, 2026
** bigint
** File description:
** bigint.h
** big integer definition
*/

#ifndef BIGINT_HEADER_H
    #define BIGINT_HEADER_H
    #include <stdint.h>
    #include <stddef.h>
    #define MAX(a, b) (a > b ? a : b)
    #define MIN(a, b) (a < b ? a : b)
    #define LIMB_BITS 32
    #define LIMB_BASE (1ULL << LIMB_BITS)
    #define LIMB_MASK (LIMB_BASE - 1)

typedef struct bigint_s {
    uint32_t *limb;
    size_t len;
    uint8_t sign;
} bigint_t;

bigint_t *duplicate_number(bigint_t const *number);
void free_number(bigint_t *number);
bigint_t *create_number(size_t len, int8_t flag);
bigint_t *addition(bigint_t *n1, bigint_t *n2);
bigint_t *subtraction(bigint_t *n1, bigint_t *n2);
char *convert_bigint_to_str(bigint_t const *number);
bigint_t *multiplication(bigint_t *n1, bigint_t *n2);
int8_t is_number_zero(bigint_t *number);
bigint_t *schoolbook_multiplication(bigint_t *a, bigint_t *b);
bigint_t *convert_str_to_bigint(char const *s);

#endif /* BIGINT HEADER DEFINED*/
