/*
** EPITECH PROJECT, 2026
** Bigint
** File description:
** fast_fourier_transform.c
** uses the fast fourier transform to do multiplication
*/

#include "bigint.h"
#include <math.h>
#include <complex.h>
#include <stdlib.h>

static void fast_fourier_transform(double complex *a, size_t n, double complex *workspace, double complex wn)
{
    if (n == 1) {
        return;
    }

    double complex *even = workspace;
    double complex *odd = workspace + n / 2;
    double complex temp = 0.0;
    double complex w = 1.0;

    for (size_t i = 0; i < n / 2; i++) {
        even[i] = a[i * 2]; // temporarly store the even and odd part in the sratch
        odd[i] = a[i * 2 + 1];
    } 

    // do the recursion
    fast_fourier_transform(even, n / 2, workspace + n, wn * wn);
    fast_fourier_transform(odd, n / 2, workspace + n, wn * wn);

    // now fill (into workspace, where the result should be)
    for (size_t k = 0; k < n / 2; k++) {
        temp = w * odd[k];

        a[k] = even[k] + temp;
        a[k + n / 2] = even[k] - temp;

        w *= wn;
    }
}

static void inverse_fourier_transform(double complex *a, size_t n, double complex *workspace, double complex wn)
{
    if (n == 1) {
        return;
    }

    double complex *even = workspace;
    double complex *odd  = workspace + n / 2;

    for (size_t i = 0; i < n / 2; i++) {
        even[i] = a[i * 2];
        odd[i]  = a[i * 2 + 1];
    }

    inverse_fourier_transform(even, n / 2, workspace + n, wn * wn);
    inverse_fourier_transform(odd,  n / 2, workspace + n, wn * wn);

    double complex w = 1.0;

    for (size_t k = 0; k < n / 2; k++) {
        double complex t = w * odd[k];

        a[k]           = even[k] + t;
        a[k + n / 2]   = even[k] - t;

        w *= wn;
    }

}

static size_t pad_to_next_power_of_2(size_t len)
{
    size_t test = 1;

    while (test < len) {
        test <<= 1;
    }
    return test;
}

static double complex get_wn(size_t n)
{
    double angle = -2.0 * M_PI / (double)n;
    return cos(angle) + I * sin(angle);
}

static double complex get_inverse_wn(size_t n)
{
    double angle = 2.0 * M_PI / (double)n;
    return cos(angle) + I * sin(angle);
}

void FFT_multiplication(uint32_t *result, uint32_t *a, size_t na, uint32_t *b, size_t nb, void *workspace)
{
    size_t coef_size = na * 2 + nb * 2 - 1;
    const size_t size = pad_to_next_power_of_2(coef_size);

    double complex *new_a = workspace;
    for (size_t i = 0; i < na; i++) {
        new_a[i * 2] = (double)(a[i] & 0xffff) + 0.0 * I;
        new_a[i * 2 + 1] = (double)(a[i] >> 16) + 0.0 * I;
    }
    for (size_t i = na * 2; i < size; i++) {
        new_a[i] = 0.0;
    }
    double complex *new_b = new_a + size;
    for (size_t i = 0; i < nb; i++) {
        new_b[i * 2] = (double)(b[i] & 0xffff) + 0.0 * I;
        new_b[i * 2 + 1] = (double)(b[i] >> 16) + 0.0 * I;
    }
    for (size_t i = nb * 2; i < size; i++) {
        new_b[i] = 0.0;
    }
    // let's go I padded my numbers.

    fast_fourier_transform(new_a, size, new_a + 2 * size, get_wn(size));
    fast_fourier_transform(new_b, size, new_a + 2 * size, get_wn(size));

    for (size_t i = 0; i < size; i++) {
        new_a[i] *= new_b[i];
    }
    
    inverse_fourier_transform(new_a, size, &new_a[size], get_inverse_wn(size));

    for (size_t i = 0; i < size; i++) {
        new_a[i] /= (double)size;
    }

    uint16_t *temp_coef = (uint16_t *)&new_a[size];
    uint64_t carry = 0;
    uint64_t coef = 0;
    for (size_t i = 0; i < coef_size; i++) {
        coef = llround(creal(new_a[i])) + carry;
        temp_coef[i] = coef & 0xffff;
        carry = coef >> 16;
    }

    while (carry) {
        temp_coef[coef_size++] = carry & 0xffff;
        carry >>= 16;
    }

    for (size_t i = 0; i < (na + nb); i++) {
        result[i] = (uint32_t)temp_coef[i * 2] | ((uint32_t)temp_coef[i * 2 + 1] << 16);
    }
}

bigint_t *FFT_caller(bigint_t *a, bigint_t *b)
{
    bigint_t *new_number = NULL;

    if (!a || !b) {
        return NULL;
    }

    const size_t size = pad_to_next_power_of_2(a->len * 2 + b->len * 2);
    uint32_t *scratch = calloc(size * 5, sizeof(double complex));

    if (!scratch) {
        return NULL;
    }

    new_number = create_number(a->len + b->len, a->sign ^ b->sign);
    if (!new_number) {
        free(scratch);
        return NULL;
    }

    FFT_multiplication(new_number->limb, a->limb, a->len, b->limb, b->len, scratch);
    
    free(scratch);

    return new_number;
}