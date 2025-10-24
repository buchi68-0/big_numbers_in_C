/*
** EPITECH PROJECT, 2025
** number_t
** File description:
** this file allows for timespec_t calculus
*/

#include "main_dependencies.h"

void timespec_treat(timespec_t *dst, const timespec_t *src)
{
    dst->tv_nsec -= src->tv_nsec;
    dst->tv_sec -= src->tv_sec;
    if (dst->tv_nsec < 0) {
        dst->tv_nsec += 1000000000L;
        dst->tv_sec -= 1;
    }
}
