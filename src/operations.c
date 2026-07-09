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
#include <sys/types.h>

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

bigint_t *schoolbook_multiplication(bigint_t *n1, bigint_t *n2)
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

static void sub_into(uint32_t *into, size_t ninto, uint32_t *arr1, size_t n1, uint32_t *arr2, size_t n2)
{
    int64_t rest = 0;
    size_t i = 0;

    for (; i < n1 && i < n2; i++) {
        rest += (int64_t)arr1[i] - (int64_t)arr2[i];
        if (rest < 0) {
            rest += LIMB_BASE;
            into[i] = rest;
            rest = -1LL;
        } else {
            into[i] = rest;
            rest = 0;
        }
    }
    for (; i < n1; i++) {
        rest += (int64_t)arr1[i];
        into[i] = rest & LIMB_MASK;
        if (rest < 0) {
            rest += LIMB_BASE;
            into[i] = rest;
            rest = -1LL;
        } else {
            into[i] = rest;
            rest = 0;
        }
    }
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

static void shift_number(uint32_t *number, size_t nnumber, int8_t nbits)
{
    uint64_t value = 0;

    for (size_t i = 0; i < nnumber; i++) {
        value += (uint64_t)number[i] << nbits;
        number[i] = value & LIMB_MASK;
        value >>= 32;
    }
}

static void divide_number_const(uint32_t *number, size_t nnumber, uint32_t constant)
{
    ssize_t i = nnumber - 1;
    uint64_t rest = 0;

    while (i >= 0) {
        rest <<= LIMB_BITS;
        rest += (uint64_t)number[i];
        number[i] = rest / (uint64_t)constant;
        rest %= (uint64_t)constant;
        i--;
    }
}

static void toom_cook(uint32_t *result, uint32_t *a, size_t na, uint32_t *b, size_t nb, uint32_t *workspace)
{

    uint32_t *sp = workspace;

    if (na < 200 && nb < 200) {
        return karatsuba(result, a, na, b, nb, workspace);
    }

    memset(result, 0, sizeof(uint32_t) * (na + nb)); // avoids shi

    size_t cut = (MAX(na, nb) + 2) / 3;
    bigint_t a0 = {.len = (cut > na ? na : cut), .limb = a};
    // a1 explained
    // Branch 1: cut > na
    //      that means a0 contains a fully. It also means a1 is empty
    // Branch 2: cut * 2 > na
    //      that means a0 and a1 will contain a fully, so we need to truncate a1's length
    // Branch 3: cut * 2 <= na
    //      that means a0 and a1 will not contain a fully, so both have length "cut"
    bigint_t a1 = {.len = (cut > na ? 0 : (cut * 2 > na ? na - cut : cut)), .limb = (cut > na ? NULL : a + cut)};
    bigint_t a2 = {.len = (cut * 2 > na) ? 0 : na - (cut * 2), .limb = (cut * 2 > na) ? NULL : a + cut * 2};
    bigint_t b0 = {.len = (cut > nb ? nb : cut), .limb = b};
    bigint_t b1 = {.len = (cut > nb ? 0 : (cut * 2 > nb ? nb - cut : cut)), .limb = (cut > nb ? NULL : b + cut)};
    bigint_t b2 = {.len = (cut * 2 > nb) ? 0 : nb - (cut * 2), .limb = (cut * 2 > nb) ? NULL : b + cut * 2};

    bigint_t r0 = {.len = a0.len + b0.len, .limb = sp, .sign = 0};
    // calculate r0
    sp += r0.len;
    toom_cook(r0.limb, a0.limb, a0.len, b0.limb, b0.len, sp);

    bigint_t r4 = {0};
    if (!is_number_zero(&b2) && !is_number_zero(&a2)) {
        r4.len = a2.len + b2.len;
        r4.limb = sp;
        sp += r4.len;
        toom_cook(r4.limb, a2.limb, a2.len, b2.limb, b2.len, sp);
    }

    bigint_t p0 = {.len = cut + 1, .limb = sp, .sign = 0};
    // p0 = a0 + a2
    // space will be reused for q0, that's why length cut
    sp += cut * 2 + 2; // this length allows me to multiply p(1) and q(1) into p0
    add_into(p0.limb, p0.len, a0.limb, a0.len, a2.limb, a2.len);

    bigint_t p_1 = {.len = p0.len + 1, .limb = sp, .sign = 0};
    // p(1) = p0 + a1
    sp += p_1.len * 2; // this allows me to put q_1 next to p_1 in memory
    add_into(p_1.limb, p_1.len, p0.limb, p0.len, a1.limb, a1.len);

    bigint_t p_m1 = {.len = p0.len, .limb = sp, .sign = 0};
    // p(-1) = p0 - a1
    sp -= p_1.len; // reputs the stack pointer after p_1
    if (get_maximum(&a1, &p0) == &a1) {
        p_m1.sign = 1;
        sub_into(p_m1.limb, p_m1.len, a1.limb, a1.len, p0.limb, p0.len);
        // reverse multiplication and change sign
    } else {
        sub_into(p_m1.limb, p_m1.len, p0.limb, p0.len, a1.limb, a1.len);
    }

    // then, p0 is not needed anymore ; so we'll use it as q0
    add_into(p0.limb, p0.len, b0.limb, b0.len, a2.limb, a2.len);
    // q0 = b0 + b2
    bigint_t q_1 = {.len = p0.len + 1, .limb = sp, .sign = 0};
    // p(1) = p0 + a1
    sp += p_m1.len + p_1.len; // puts the stack pointer after p_m1
    add_into(q_1.limb, q_1.len, p0.limb, p0.len, b1.limb, b1.len);

    bigint_t q_m1 = {.len = p0.len, .limb = sp, .sign = 0};
    // p(-1) = p0 - a1
    sp += q_m1.len + 4; // the +4 is a simbolic space move to be able to mulitply p_m2 and p_q2 into this space later
    if (get_maximum(&b1, &p0) == &b1) {
        q_m1.sign = 1;
        sub_into(q_m1.limb, q_m1.len, b1.limb, b1.len, p0.limb, p0.len);
        // reverse multiplication and change sign
    } else {
        sub_into(q_m1.limb, q_m1.len, p0.limb, p0.len, b1.limb, b1.len);
    }

    // we multiply q_1 and p_1 into the p0 space
    bigint_t r_1 = {.limb = p0.limb, .len = q_1.len + p_1.len, .sign = 0};
    toom_cook(r_1.limb, q_1.limb, q_1.len, p_1.limb, p_1.len, sp);

    // Now, we don't need p(1) and q(1), so we'll multiply p(-1) and q(-1) into the p_1 and r_1 space
    bigint_t r_m1 = {.limb = p_1.limb, .len = q_m1.len + p_m1.len, .sign = q_m1.sign ^ p_m1.sign};
    toom_cook(r_m1.limb, p_m1.limb, p_m1.len, q_m1.limb, q_m1.len, sp);


    // then we do p(-2) and q(-2)
    bigint_t p_m2 = {.len = cut + 2, .limb = sp, .sign = 0};
    sp += p_m2.len;
    if (p_m1.sign == 0) {
        add_into(p_m2.limb, p_m2.len, p_m1.limb, p_m1.len, a2.limb, a2.len);
    } else {
        if (get_maximum(&p_m1, &a2) == &a2) {
            sub_into(p_m2.limb, p_m2.len, a2.limb, a2.len, p_m1.limb, p_m1.len);
        } else {
            p_m2.sign = 1;
            sub_into(p_m2.limb, p_m2.len, p_m1.limb, p_m1.len, a2.limb, a2.len);
        }
    }
    shift_number(p_m2.limb, p_m2.len, 1);
    if (p_m2.sign == 0) {
        // if p_m2 is positive

        if (get_maximum(&p_m2, &a0) == &a0) {
            // cannot handle a0
            sub_into(p_m2.limb, p_m2.len, a0.limb, a0.len, p_m2.limb, p_m2.len);
            p_m2.sign = 1;
            // reverse subtraction, p_m2 becomes negative
        } else {
            sub_assign(p_m2.limb, a0.limb, a0.len);
        }
    } else {
        // then it is - (-) + => -(+ (+) +)
        add_assign(p_m2.limb, a0.limb, a0.len);
    }


    bigint_t q_m2 = {.len = cut + 2, .limb = sp, .sign = 0};
    sp += q_m2.len;
    if (q_m1.sign == 0) {
        add_into(q_m2.limb, q_m2.len, q_m1.limb, q_m1.len, b2.limb, b2.len);
    } else {
        if (get_maximum(&q_m1, &b2) == &b2) {
            sub_into(q_m2.limb, q_m2.len, b2.limb, b2.len, q_m1.limb, q_m1.len);
        } else {
            q_m2.sign = 1;
            sub_into(q_m2.limb, q_m2.len, q_m1.limb, q_m1.len, b2.limb, b2.len);
        }
    }
    shift_number(q_m2.limb, q_m2.len, 1);
    if (q_m2.sign == 0) {
        // if p_m2 is positive

        if (get_maximum(&q_m2, &b0) == &b0) {
            // cannot handle a0
            sub_into(q_m2.limb, q_m2.len, b0.limb, b0.len, q_m2.limb, q_m2.len);
            q_m2.sign = 1;
            // reverse subtraction, p_m2 becomes negative
        } else {
            sub_assign(q_m2.limb, b0.limb, b0.len);
        }
    } else {
        // then it is - (-) + => -(+ (+) +)
        add_assign(q_m2.limb, b0.limb, b0.len);
    }

    // now we multiply them into the p_m1 space
    bigint_t r_m2 = {.limb = p_m1.limb, .len = p_m2.len + q_m2.len, .sign = p_m2.sign ^ q_m2.sign};
    toom_cook(r_m2.limb, p_m2.limb, p_m2.len, q_m2.limb, q_m2.len, sp);

    // since we don't need q_m2 and p_m2 anymore, we're gonna move the sp so there's more space
    sp -= p_m2.len;
    sp -= q_m2.len;

    // now onto the addition phase
    bigint_t r3 = {.limb = sp, .len = r_m2.len + 1, .sign = r_m2.sign};
    sp += r3.len;
    // r3 = (r(-2) - r(1)) / 3
    if (r3.sign != r_1.sign) {
        add_into(r3.limb, r3.len, r_m2.limb, r_m2.len, r_1.limb, r_1.len);
    } else {
        if (get_maximum(&r_m2, &r_1) == &r_1) {
            r3.sign ^= 1;
            sub_into(r3.limb, r3.len, r_1.limb, r_1.len, r_m2.limb, r_m2.len);
        } else {
            sub_into(r3.limb, r3.len, r_m2.limb, r_m2.len, r_1.limb, r_1.len);
        }
    }
    divide_number_const(r3.limb, r3.len, 3);

    bigint_t r1 = {.limb = sp, .len = r_1.len + 1, .sign = r_1.sign};
    sp += r1.len;
    // r1 = (r(1) - r(-1)) / 2
    if (r1.sign != r_m1.sign) {
        add_into(r1.limb, r1.len, r_1.limb, r_1.len, r_m1.limb, r_m1.len);
    } else {
        if (get_maximum(&r_m1, &r_1) == &r_m1) {
            r1.sign ^= 1;
            sub_into(r1.limb, r1.len, r_m1.limb, r_m1.len, r_1.limb, r_1.len);
        } else {
            sub_into(r1.limb, r1.len, r_1.limb, r_1.len, r_m1.limb, r_m1.len);
        }        
    }

    bigint_t r2 = {.limb = sp, .len = r_1.len + 2, .sign = r_m1.sign};
    sp += r2.len;
    // r2 = r(-1) - r(0)
    if (r2.sign != 0) { // (we know r0 is positive)
        add_into(r2.limb, r2.len, r_m1.limb, r_m1.len, r0.limb, r0.len);
    } else {
        if (get_maximum(&r_m1, &r0) == &r0) {
            r2.sign ^= 1;
            sub_into(r2.limb, r2.len, r0.limb, r0.len, r_m1.limb, r_m1.len);
        } else {
            sub_into(r2.limb, r2.len, r_m1.limb, r_m1.len, r0.limb, r0.len);
        }
    }

    // r3 = (r2 - r3) / 2 + 2r4
    // fortunately, we don't need r(1), r(-1) nor r(-2), nor p0
    // so we're doing that in this space

    // set up 2r4
    add_into(p0.limb, r4.len * 2 + 1, r4.limb, r4.len, r4.limb, r4.len);

    // make the subtraction
    if (r2.sign != r3.sign) {
        add_assign(r3.limb, r2.limb, r2.len);
    } else {
        if (get_maximum(&r2, &r3) == &r2) {
            r3.sign ^= 1;
            sub_into(r3.limb, r3.len, r2.limb, r2.len, r3.limb, r3.len);
        } else {
            sub_assign(r3.limb, r2.limb, r2.len);
        }
    }
    add_assign(r3.limb, p0.limb, r4.len * 2 + 1);

    // r2 = r2 + r1 - r4
    if (r2.sign == r1.sign) {
        add_assign(r2.limb, r1.limb, r1.len);
    } else {
        if (get_maximum(&r2, &r1) == &r1) {
            r2.sign ^= 1;
            sub_into(r2.limb, r2.len, r1.limb, r1.len, r2.limb, r2.len);
        } else {
            sub_assign(r2.limb, r1.limb, r1.len);
        }
    }

    // r1 = r1 - r3
    if (r1.sign != r3.sign) {
        add_assign(r1.limb, r3.limb, r3.len);
    } else {
        if (get_maximum(&r3, &r1) == &r3) {
            r1.sign ^= 1;
            sub_into(r1.limb, r1.len, r3.limb, r3.len, r1.limb, r1.len);
        } else {
            sub_assign(r1.limb, r3.limb, r3.len);
        }
    }

    // now it's time to add that up
    memcpy(result, r0.limb, r0.len * sizeof(uint32_t));
    memcpy(result + 2 * cut, r2.limb, r2.len * sizeof(uint32_t));
    memcpy(result + 4 * cut, r4.limb, r4.len * sizeof(uint32_t));
    add_assign(result + cut, r1.limb, r1.len);
    add_assign(result + cut * 3, r3.limb, r3.len);
}

bigint_t *multiplication(bigint_t *a, bigint_t *b)
{
    if (!a || !b)
        return NULL;
    size_t n = MAX(a->len, b->len);

    uint32_t *scratch = calloc(8 * n, sizeof(uint32_t));

    if (!scratch) {
        return NULL;
    }

    bigint_t *res = create_number(a->len + b->len, a->sign ^ b->sign);

    if (!res) {
        free(scratch);
        return NULL;
    }

    //toom_cook(res->limb,
    karatsuba(res->limb,
              a->limb, a->len,
              b->limb, b->len,
              scratch);

    free(scratch);

    return res;
}