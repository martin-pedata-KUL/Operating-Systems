#define _GNU_SOURCE



#include "dplist.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct {
    int id;
    char* name;
} my_element_t;

void * element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name,"%s",((my_element_t*)element)->name); //asprintf requires _GNU_SOURCE
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

void element_free(void ** element) {
    free((((my_element_t*)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}

void yourtest1() {
        // Test free NULL, don't use callback
        dplist_t *list = NULL;
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free NULL, use callback
        list = NULL;
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, don't use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");


    printf("---------- Start of Martin's Tests -----------\n");

    printf("Start of testss without callback ... \n \n \n");

        my_element_t *e1 = malloc(sizeof(my_element_t));
        e1->id = 1; e1->name = "alpha";
        my_element_t *e2 = malloc(sizeof(my_element_t));
        e2->id = 2; e2->name = "beta";
        my_element_t *e3 = malloc(sizeof(my_element_t));
        e3->id = 3; e3->name = "gamma";
        my_element_t *e4 = malloc(sizeof(my_element_t));
        e4->id = 4; e4->name = "delta";

        dplist_t *list_martin = dpl_create(element_copy, element_free, element_compare);
        ck_assert_msg(list_martin != NULL, "Failure: expected non-NULL list after creation");

        // Insert elements at different positions. Including at negative positions and at positions larger than list size. These are not deep copies.
        list_martin = dpl_insert_at_index(list_martin, e1, 0, false);     // Head of the list (id = 1)
        list_martin = dpl_insert_at_index(list_martin, e2, -1, false);    // New head of the list (id = 2)
        list_martin = dpl_insert_at_index(list_martin, e3, 99, false);    // Tail (id = 3)
        list_martin = dpl_insert_at_index(list_martin, e4, 1, false);     // Middle (id = 4)

        ck_assert_msg(dpl_size(list_martin) == 4, "Failure: expected list size of 4 after inserts");


        // We verify insertion order by looking at the elements' id
        my_element_t *a = (my_element_t *)dpl_get_element_at_index(list_martin, 0);
        my_element_t *b = (my_element_t *)dpl_get_element_at_index(list_martin, 1);
        my_element_t *c = (my_element_t *)dpl_get_element_at_index(list_martin, 2);
        my_element_t *d = (my_element_t *)dpl_get_element_at_index(list_martin, 3);

        ck_assert_msg(a->id == 2, "Failure: expected first element: id=2 (inserted with -1 index)");
        ck_assert_msg(b->id == 4, "Failure: expected second element: id=4");
        ck_assert_msg(c->id == 1, "Failure: expected third element: id=1");
        ck_assert_msg(d->id == 3, "Failure: expected last element: id=3");

        // Verify that the inserted elements are not deep copies, but rather point to the same addresses as the original elements in scope defined above (e1,e2,e3,e4) by comparing the addresses.
        my_element_t *e1_list = (my_element_t *)dpl_get_element_at_index(list_martin, 2);
        my_element_t *e2_list = (my_element_t *)dpl_get_element_at_index(list_martin, 0);
        ck_assert_msg(e1_list == e1 && e2_list == e2, "Failure: the original and list elements do not point to the same addresses in memory");

        // Modify originals and check that list elements do not change
        e1->id = 999;
        ck_assert_msg(e1->id == e1_list->id,"Failure: The list element is not affected by modification to the original");

        // Remove elements
        list_martin = dpl_remove_at_index(list_martin, 1, false); // remove element with id=4
        ck_assert_msg(dpl_size(list_martin) == 3, "Failure: expected list size 3 after removal");

        // Check new order of elements in the list
        a = (my_element_t *)dpl_get_element_at_index(list_martin, 0);
        b = (my_element_t *)dpl_get_element_at_index(list_martin, 1);
        c = (my_element_t *)dpl_get_element_at_index(list_martin, 2);
        ck_assert_msg(a->id == 2 && b->id == 999 && c->id == 3, "Failure: wrong element order after removal");

        dpl_free(&list_martin, false);
        ck_assert_msg(list_martin == NULL, "Failure: expected list to be NULL after free() execution");

        free(e1);
        free(e2);
        free(e3);
        free(e4);

    printf("Completion of no-Callback Tests \n \n");

    printf("Start of Callback tests...\n \n \n");

        my_element_t *orig1 = malloc(sizeof(my_element_t));
        orig1->id = 100;
        orig1->name = "alpha";

        my_element_t *orig2 = malloc(sizeof(my_element_t));
        orig2->id = 200;
        orig2->name = "beta";

        list = dpl_create(element_copy, element_free, element_compare);
        ck_assert_msg(list != NULL, "Failure: expected non-NULL list");

        // Insert deep copies
        list = dpl_insert_at_index(list, orig1, 0, true);
        list = dpl_insert_at_index(list, orig2, 1, true);

        ck_assert_msg(dpl_size(list) == 2, "Failure: expected list size of 2 after deep copy inserts");

        // Verify that the inserted elements are deep copies by comparing the addresses with their original elements.
        my_element_t *copy1 = (my_element_t *)dpl_get_element_at_index(list, 0);
        my_element_t *copy2 = (my_element_t *)dpl_get_element_at_index(list, 1);
        ck_assert_msg(copy1 != orig1 && copy2 != orig2, "Failure: deep copy returned same pointer");
        ck_assert_msg(copy1->id == 100 && copy2->id == 200, "Failure: expected correct deep copy data");

        // Modify originals and check that copies remain unchanged
        orig1->id = 999;
        ck_assert_msg(orig1->id != copy1->id,"Failure: deep copy id affected by original modification");

        // Comparison Callback tests
        int cmp_equal = element_compare(copy1, copy1);
        int cmp_diff = element_compare(copy1, copy2);
        ck_assert_msg(cmp_equal == 0, "Failure: element_compare did not detect equality");
        ck_assert_msg(cmp_diff != 0, "Failure: element_compare did not detect difference");

        // Free list with callback
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected list to be NULL after dpl_free");

        free(orig1);
        free(orig2);

    printf("Completion of Callback Tests \n");

    //Note: to test whether the free() memory callback worked as expected, I ran valgrind and checked that all allocated memory was released.
    // Since both deep copies and shallow copies have been used, the fact that everything is freed correctly, proves the code is working as expected.

    printf("---------- Completion of Martin's Tests :) -----------\n");


}




int main(void) {

    yourtest1();
    return 0;
}
