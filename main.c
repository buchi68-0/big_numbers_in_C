/*
** EPITECH PROJECT, 2026
** test
** File description:
** main.c
** test
*/

#include "bigint.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void print_time(char const *s, struct timespec *time1, struct timespec *time2)
{
    time2->tv_sec -= time1->tv_sec;
    time2->tv_nsec -= time1->tv_nsec;
    if (time2->tv_nsec < 0) {
        time2->tv_nsec += 1000000000L;
        time2->tv_sec--;
    }
    if (s) {
        printf("%s time: %ld.%.9ld\n", s, time2->tv_sec, time2->tv_nsec);
    } else {
        printf("time: %ld.%.9ld\n", time2->tv_sec, time2->tv_nsec);
    }
    return;
}

static void huge_test(void)
{
    bigint_t *n1 = create_number(200000, 0);
    bigint_t *n2 = create_number(200000, 0);
    struct timespec t1 = {0};
    struct timespec t2 = {0};

    if (!n1 || !n2)
        return;
    for (size_t i = 0; i < 200000; i++) {
        n1->limb[i] = LIMB_MASK;
        n2->limb[i] = LIMB_MASK;
    }
    /*char a[] = "10";
    timespec_get(&t1, TIME_UTC);
    bigint_t *new_number = convert_str_to_bigint(a);
    timespec_get(&t2, TIME_UTC);
    print_time("conversion string to number", &t1, &t2);*/
    /*timespec_get(&t1, TIME_UTC);
    printf("n1: %s\nn2: %s\n", convert_bigint_to_str(n1), convert_bigint_to_str(n2));
    timespec_get(&t2, TIME_UTC);
    print_time("time for conversions", &t1, &t2);*/
    /*timespec_get(&t1, TIME_UTC);
    bigint_t *n3 = schoolbook_multiplication(n1, n2);
    timespec_get(&t2, TIME_UTC);
    print_time("schoolbook mul", &t1, &t2); */
    timespec_get(&t1, TIME_UTC);
    bigint_t *n4 = multiplication(n1, n2);
    timespec_get(&t2, TIME_UTC);
    print_time("karatsuba mul", &t1, &t2);
    timespec_get(&t1, TIME_UTC);
    convert_bigint_to_str(n4);
    //printf("n3: %s\n", convert_bigint_to_str(n4));
    //printf("n4: %s\n", convert_bigint_to_str(n4));
    timespec_get(&t2, TIME_UTC);
    print_time("time for conversions", &t1, &t2);
}

int main(void)
{
    bigint_t *n1 = create_number(2, 0);
    bigint_t *n2 = create_number(3, 0);
    bigint_t *n3 = create_number(3, 1);

    n1->limb[0] = 256;
    n2->limb[0] = 3;
    n3->limb[0] = 0;
    n3->limb[1] = 0;
    n3->limb[2] = 0x80000000U;

    printf("n1: %s\n", convert_bigint_to_str(n1));
    printf("n2: %s\n", convert_bigint_to_str(n2));
    printf("n1+n2: %s\n", convert_bigint_to_str(addition(n1, n2)));
    printf("n1-n2: %s\n", convert_bigint_to_str(subtraction(n1, n2)));
    printf("n2-n1: %s\n", convert_bigint_to_str(subtraction(n2, n1)));
    printf("n3: %s\n", convert_bigint_to_str(n3));
    printf("n3+n2: %s\n", convert_bigint_to_str(addition(n3, n2)));
    printf("n3-n2: %s\n", convert_bigint_to_str(subtraction(n3, n2)));
    printf("n2-n3: %s\n", convert_bigint_to_str(subtraction(n2, n3)));
    printf("n2*n3: %s\n", convert_bigint_to_str(multiplication(n2, n3)));
    printf("n2*n1: %s\n", convert_bigint_to_str(multiplication(n2, n1)));

    huge_test();
    return 0;
}