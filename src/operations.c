/*
** PERSONNAL PROJECT, 2026
** bigint
** File description:
** addition.c
** does addition of 2 numbers
*/

#include "bigint.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// starts from the end up towards the beggining
static bigint_t *get_maximum(bigint_t *n1, bigint_t *n2)
{
    size_t size1 = n1->len;
    size_t size2 = n2->len;
    size_t i = MAX(size1, size2) - 1;

    if (size2 > size1) {
        while (i >= size1) {
            if (n2->limb[i] != 0)
                return n2;
            i--;
        }
    } else {
        while (i >= size2) {
            if (n1->limb[i] != 0)
                return n1;
            i--;
        }
    }
    while (i >= 0) {
        if (n1->limb[i] != n2->limb[i]) {
            if (n1->limb[i] > n2->limb[i])
                return n1;
            return n2;
        }
        if (i == 0)
            return n1;
        i--;
    }
    return n1;
}

// n1 > n2
static void unsigned_addition(bigint_t *into, bigint_t *n1, bigint_t *n2)
{
    uint64_t rest = 0;
    size_t i = 0;

    for (; i < n2->len && i < n1->len; i++) {
        rest += n1->limb[i] + n2->limb[i];
        into->limb[i] = (uint32_t)(rest % ((uint64_t)1 << 32));
        rest >>= 32;
    }
    while (i < n1->len) {
        rest += n1->limb[i];
        into->limb[i] = rest % ((uint64_t)1 << 32);
        rest >>= 32;
        i++;
    }
    into->limb[i] = rest;
    return;
}

// n1 > n2
static void unsigned_subtraction(bigint_t *into, bigint_t *n1, bigint_t *n2)
{
    int64_t rest = 0;
    int8_t is_reduced = 0;
    size_t i = 0;

    for (; i < n2->len && i < n1->len; i++) {
        rest = (int64_t)n1->limb[i] - (int64_t)n2->limb[i];
        if (is_reduced) {
            rest -= 1LL;
        }
        if (rest < 0LL) {
            is_reduced = 1;
            rest += (int64_t)LIMB_BASE;
        }
        into->limb[i] = (uint32_t)rest;
    }
    while (i < n1->len) {
        rest = n1->limb[i];
        if (is_reduced) {
            rest -= 1LL;
            is_reduced = 0;
        }
        if (rest < 0) {
            is_reduced = 1;
            rest += (int64_t)LIMB_BASE;
        }
        into->limb[i] = (uint32_t)rest;
        i++;
    }
    return;
}

bigint_t *addition(bigint_t *n1, bigint_t *n2)
{
    bigint_t *max = NULL;
    bigint_t *new_number = NULL;

    if (!n1 || !n2) {
        return NULL;
    }
    if (!(n1->sign ^ n2->sign)) {
        max = get_maximum(n1, n2);
        new_number = create_number(max->len + 1, max->sign);
        if (!new_number) {
            return NULL;
        }
        unsigned_addition(new_number, max, (max == n1 ? n2 : n1));
    } else {
        max = get_maximum(n1, n2);
        new_number = create_number(max->len + 1, max->sign);
        if (!new_number) {
            return NULL;
        }
        unsigned_subtraction(new_number, max, (max == n1 ? n2 : n1));
        if (is_number_zero(new_number)) {
            new_number->sign = 0;
        }
    }
    return new_number;
}

bigint_t *subtraction(bigint_t *n1, bigint_t *n2)
{
    bigint_t *new_number = NULL;
    bigint_t *max = NULL;

    if (!n1 || !n2) {
        return NULL;
    }
    if (!(n1->sign ^ n2->sign)) {
        max = get_maximum(n1, n2);
        // 1 - 2 has not sign + even though first and second is +, it depends on weather the first or the second is the biggest
        new_number = create_number(max->len + 1, (max == n1 ? max->sign : max->sign ^ 1));
        if (!new_number)
            return NULL;
        unsigned_subtraction(new_number, max, (max == n1 ? n2 : n1));
        if (is_number_zero(new_number)) {
            new_number->sign = 0;
        }
    } else {
        max = get_maximum(n1, n2);
        // 1 - (-2) has sign + and not minus, it doesn't depend of the max, but on the first number
        new_number = create_number(max->len + 1, n1->sign);
        if (!new_number) {
            return NULL;
        }
        unsigned_addition(new_number, max, (max == n1 ? n2 : n1));
    }
    return new_number;
}

static void add_carry(uint32_t *arr, size_t j, uint64_t carry)
{
    uint64_t lo = (carry & LIMB_MASK) + arr[j];

    arr[j] = lo & LIMB_MASK;
    carry = (carry >> LIMB_BITS) + (lo >> LIMB_BITS);
    j++;
    while (carry != 0) {
        carry += arr[j];
        arr[j] = carry & LIMB_MASK;
        carry >>= LIMB_BITS;
        j++;
    }
}

bigint_t *multiplication(bigint_t *n1, bigint_t *n2)
{
    bigint_t *new_number = NULL;

    if (!n1 || !n2) {
        return NULL;
    }
    new_number = create_number(n1->len + n2->len + 1, n1->sign ^ n2->sign);
    for (size_t i = 0; i < n1->len; i++) {
        if (!n1->limb[i])
            continue;
        uint64_t carry = 0;
        for (size_t j = 0; j < n2->len; j++) {
            uint64_t prod = (uint64_t)n1->limb[i] * (uint64_t)n2->limb[j];
            uint64_t lo = (prod & (uint64_t)LIMB_MASK) + (uint64_t)new_number->limb[i + j] + (carry & (uint64_t)LIMB_MASK);

            new_number->limb[i + j] = lo & (uint64_t)LIMB_MASK;
            carry = (lo >> LIMB_BITS) + (prod >> LIMB_BITS) + (carry >> LIMB_BITS);
        }
        add_carry(new_number->limb, i + n2->len, carry);
    }
    return new_number;
}

static void multiplication_into(uint32_t *result, uint32_t *a, size_t na, uint32_t *b, size_t nb)
{
    for (size_t i = 0; i < na; i++) {
        if (!a[i])
            continue;
        uint64_t carry = 0;
        for (size_t j = 0; j < nb; j++) {
            uint64_t prod = (uint64_t)a[i] * (uint64_t)b[j];
            uint64_t lo = (prod & (uint64_t)LIMB_MASK) + (uint64_t)result[i + j] + (carry & (uint64_t)LIMB_MASK);

            result[i + j] = lo & (uint64_t)LIMB_MASK;
            carry = (lo >> LIMB_BITS) + (prod >> LIMB_BITS) + (carry >> LIMB_BITS);
        }
        add_carry(result, i + nb, carry);
    }
}

static void add_assign(uint32_t *into, uint32_t *from, size_t nfrom)
{
    uint64_t rest = 0;
    size_t i = 0;

    for (; i < nfrom; i++) {
        rest += (uint64_t)into[i] + (uint64_t)from[i];
        into[i] = rest & LIMB_MASK;
        rest >>= LIMB_BITS;
        //printf("value = %x, carry = %x\n", into[i], rest);
    }
    while (rest != 0) {
        rest += into[i];
        into[i] = rest & LIMB_MASK;
        rest >>= LIMB_BITS;
        i++;
    }
}

static void sub_assign(uint32_t *into, uint32_t *from, size_t nfrom)
{
    int64_t rest = 0;
    size_t i = 0;
    int8_t is_reduced = 0;

    for (; i < nfrom; i++) {
        rest = (int64_t)into[i] - (int64_t)from[i];
        if (is_reduced) {
            rest -= 1LL;
            is_reduced = 0;
        }
        if (rest < 0) {
            is_reduced = 1;
            rest += LIMB_BASE;
        }
        into[i] = rest;
    }
    while (is_reduced) {
        rest = (int64_t)into[i] - 1LL;
        is_reduced = 0;
        if (rest < 0) {
            is_reduced = 1;
            rest += LIMB_BASE;
        }
        into[i] = rest;
        i++;
    }
}

static void add_into(uint32_t *into, size_t ninto, uint32_t *arr1, size_t n1, uint32_t *arr2, size_t n2)
{
    uint64_t rest = 0;
    size_t i = 0;

    for (; i < n1 && i < n2; i++) {
        rest += (uint64_t)arr1[i] + (uint64_t)arr2[i];
        into[i] = rest & LIMB_MASK;
        rest >>= LIMB_BITS;
        //printf("value = %x, carry = %x\n", into[i], rest);
    }
    for (; i < n1; i++) {
        rest += (uint64_t)arr1[i];
        into[i] = rest & LIMB_MASK;
        rest >>= LIMB_BITS;
        //printf("value = %x, carry = %x\n", into[i], rest);
    }
    into[i] = rest;
    i++;
    while (i < ninto) {
        into[i] = 0;
        i++;
    }
}

static void karatsuba(uint32_t *result, uint32_t *a, size_t na, uint32_t *b, size_t nb, uint32_t *workspace)
{
    uint32_t *sp = workspace;

    memset(result, 0, sizeof(uint32_t) * (na + nb)); // avoids shi

    if (na < 35 && nb < 35) { // arbitrary constant, will do for now
        return multiplication_into(result, a, na, b, nb);
    }

    // creates cuts
    size_t cut = (MAX(na, nb) + 1) / 2; // the +1 ensures that the lower part is superior to the upper part
    bigint_t a0 = {.len = (cut > na ? na : cut), .limb = a};
    bigint_t a1 = {.len = (cut > na ? 0 : na - cut), .limb = (cut > na ? NULL : a + cut)};
    bigint_t b0 = {.len = (cut > nb ? nb : cut), .limb = b};
    bigint_t b1 = {.len = (cut > nb ? 0 : nb - cut), .limb = (cut > nb ? NULL : b + cut)};


    // make the addition
    bigint_t sum_a = {.len = a0.len + 1, .limb = sp};
    sp += sum_a.len;
    bigint_t sum_b = {.len = b0.len + 1, .limb = sp};
    sp += sum_b.len;
    add_into(sum_a.limb, sum_a.len, a0.limb, a0.len, a1.limb, a1.len);
    add_into(sum_b.limb, sum_b.len, b0.limb, b0.len, b1.limb, b1.len);

    // multiplications
    bigint_t mul_sum = {.len = sum_a.len + sum_b.len, .limb = sp};
    sp += mul_sum.len;
    karatsuba(mul_sum.limb, sum_a.limb, sum_a.len, sum_b.limb, sum_b.len, sp);

    bigint_t mul_lo = {.len = a0.len + b0.len, .limb = sp};
    sp += mul_lo.len;
    karatsuba(mul_lo.limb, a0.limb, a0.len, b0.limb, b0.len, sp);
    
    memcpy(result, mul_lo.limb, mul_lo.len * sizeof(uint32_t)); // add mul_lo into the result (result being 0, we can just copy)
    sub_assign(mul_sum.limb, mul_lo.limb, mul_lo.len); // subtract mul_lo from mul_sum

    // then; mul_lo is never used again !
    sp -= mul_lo.len;

    bigint_t mul_hi = {0};
    if (!is_number_zero(&a1) && !is_number_zero(&b1)) {
        mul_hi = (bigint_t){.len = a1.len + b1.len, .limb = sp};
        sp += mul_hi.len;

        karatsuba(mul_hi.limb, a1.limb, a1.len, b1.limb, b1.len, sp);
        memcpy(result + 2 * cut, mul_hi.limb, mul_hi.len * sizeof(uint32_t)); // add the high part into the result
        sub_assign(mul_sum.limb, mul_hi.limb, mul_hi.len); // subtract mul_hi from mul_sum
    }
    // so after that mul_sum = (mul_sum - mul_hi - mul_lo)

    // now we can add the middle part (moved by cut)
    add_assign(result + cut, mul_sum.limb, mul_sum.len);
}

bigint_t *advanced_multiplication(bigint_t *a, bigint_t *b)
{
    if (!a || !b)
        return NULL;
    size_t n = a->len + b->len;

    uint32_t *scratch = calloc(10 * n, sizeof(uint32_t));

    if (!scratch) {
        return NULL;
    }

    bigint_t *res = create_number(a->len + b->len, a->sign ^ b->sign);

    if (!res) {
        free(scratch);
        return NULL;
    }

    karatsuba(res->limb,
              a->limb, a->len,
              b->limb, b->len,
              scratch);

    free(scratch);

    return res;
}