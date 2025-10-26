#define _GNU_SOURCE
/**
 * \author Bert Lagaisse
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "dplist.h"

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}
int main(void)
{
    dplist_t *numbers = NULL;
    numbers = dpl_create();

    ck_assert_msg(numbers != NULL, "numbers = NULL, List not created");
    ck_assert_msg(dpl_size(numbers) == 0, "Numbers may not contain elements.");

    dpl_insert_at_index(numbers, "berry", 0); // "" Represents a string literal / character array. '' Represents a unique and bizzarre integer value using ASCII.
    ck_assert_msg(dpl_size(numbers) == 1, "Numbers must contain 1 element.");

    dpl_insert_at_index(numbers, NULL, -1);
    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must contain 2 elements.");

    dpl_insert_at_index(numbers, "banana", 100);
    ck_assert_msg(dpl_size(numbers) == 3, "Numbers must contain 3 elements.");

    numbers = dpl_remove_at_index(numbers,1);
    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must contain 2 elements.");

    char *value = "berry";
    char *element = dpl_get_element_at_index(numbers, 0);

    ck_assert_msg(element != NULL && strcmp(element, value) == 0, "The value should be 'berry'"); //strcmp() fails if one of the arguments is NULL

    dpl_insert_at_index(numbers, "apple", 100);
    dpl_insert_at_index(numbers, "cherry", 100);
    dpl_insert_at_index(numbers, "blossom", 100);
    dpl_insert_at_index(numbers, "wow", 100);
    ck_assert_msg(dpl_size(numbers) == 6, "Numbers must contain 6 elements.");

    dpl_free(&numbers);

    return 0;
}