/*
** EPITECH PROJECT, 2025
** bit int in C
** File description:
** this file allows for basic operations,
** such as subtraction and addition
*/

#include "number_basic_operation_dependencies.h"

static void set_retenue(int64_t *intermidiate, uint32_t *retenue)
{
    if (*(intermidiate) < 0) {
        *(retenue) = 1;
        *(intermidiate) = *(intermidiate) + 4294967296;
        return;
    }
    *(retenue) = 0;
    return;
}

//case where N1 > N2. Works even if N2->length > N1->length
static number_t sub_uns_numbers(number_t *n1, number_t *n2)
{
    number_t result = my_create_number(n1->length);
    uint32_t retenue = 0;
    int64_t intermidiate = 0;

    for (uint64_t i = 0; i < n1->length; i++) {
        intermidiate = n1->limbs[n1->length - i - 1];
        intermidiate -= retenue;
        if (i < n2->length) {
            intermidiate -= n2->limbs[n2->length - i - 1];
        }
        set_retenue(&intermidiate, &retenue);
        result.limbs[result.length - i - 1] = intermidiate;
    }
    check_useless_zero(&result);
    return result;
}

//case where N1 > N2; works even if N2.length > N1.length
static number_t add_uns_numbers(number_t *n1, number_t *n2)
{
    number_t result = my_create_number(n1->length + 1);
    uint64_t retenue = 0;

    for (uint64_t i = 0; i < n1->length; i++) {
        retenue += n1->limbs[n1->length - i - 1];
        if (i < n2->length) {
            retenue += n2->limbs[n2->length - i - 1];
        }
        result.limbs[n1->length - i] = retenue & 4294967295;
        retenue = retenue >> 32;
    }
    result.limbs[0] = retenue;
    check_useless_zero(&result);
    return result;
}

static int handles_diff(int8_t *flag, number_t *n1, number_t *n2, uint64_t i)
{
    int64_t difference = 0;

    if (i < n1->length) {
        difference += n1->limbs[n1->length - i - 1];
    }
    if (i < n2->length) {
        difference -= n2->limbs[n2->length - i - 1];
    }
    if (difference == 0) {
        return 0;
    }
    if (difference < 0) {
        *flag = 1;
    }
    return 1;
}

/*returns a number_t ** where [0] is (pointer to) max and [1] is min
** this protection is not working btw (6th line in function)
** because I would've already have accessed 0x8 if
** either n1 or n2 were NULL
** (although, it is protecting the malloc at least)*/
static number_t **get_bigger_number(number_t *n1, number_t *n2)
{
    int64_t n2_index = n2->length;
    int64_t n1_index = n1->length;
    number_t **returned = (number_t **)malloc(2 * sizeof(number_t *));
    int8_t flag = 0;

    if (n1 == NULL || n2 == NULL || returned == NULL)
        return NULL;
    for (; n2_index > n1_index; n2_index--) {
        if (handles_diff(&flag, n1, n2, n2_index)) {
            n1_index = 0;
            break;
        }
    }
    for (; n1_index >= 0; n1_index--) {
        if (handles_diff(&flag, n1, n2, n1_index))
            break;
    }
    returned[flag] = n1;
    returned[flag ^ 1] = n2;
    return returned;
}

number_t sub_numbers(number_t *n1, number_t *n2)
{
    number_t result = {NULL, 0, 0};
    number_t **max = get_bigger_number(n1, n2);

    if (n2->sign ^ n1->sign) {
        result = add_uns_numbers(max[0], max[1]);
        result.sign = n1->sign;
        free(max);
        return result;
    }
    result = sub_uns_numbers(max[0], max[1]);
    result.sign = (max[0] == n2);
    free(max);
    check_useless_zero(&result);
    return result;
}

number_t add_numbers(number_t *n1, number_t *n2)
{
    number_t result = {NULL, 0, 0};
    number_t **max = get_bigger_number(n1, n2);

    if (n2->sign ^ n1->sign) {
        result = sub_uns_numbers(max[0], max[1]);
        result.sign = max[0]->sign;
        free(max);
        check_useless_zero(&result);
        return result;
    }
    result = add_uns_numbers(max[0], max[1]);
    result.sign = max[0]->sign;
    free(max);
    return result;
}
