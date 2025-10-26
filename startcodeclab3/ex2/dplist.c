/**
 * \author Jeroen Van Aken, Bert Lagaisse, Ludo Bruynseels
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "dplist.h"



/*
 * The real definition of struct list / struct node
 */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element;
};

struct dplist {
    dplist_node_t *head;
    // more fields will be added later
};

dplist_t *dpl_create() {
    dplist_t *list;
    list = malloc(sizeof(dplist_t));
    list->head = NULL;
    return list;
}

void dpl_free(dplist_t **list) {
    if (*list == NULL) {
        list = NULL;
        return;
    }
    if ((*list)->head != NULL) {
        int size = dpl_size(*list);
        for (int i = 0; i < size; i++) {
            *list = dpl_remove_at_index(*list,i);
        }
    }
    free(*list);
    list = NULL;
    //Do extensive testing with vagrind.
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/


dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));
    //list_node->element = malloc(sizeof(element_t)); /// DO NOT ALLOCATE DYNAMIC MEMORY FOR THE ELEMENT. THIS ELEMENT IS A REFERENCE TO READ ONLY MEMORY ("BANANA" ELEMENT WONT CHANGE AT RUNTIME) WHICH IS KNOWN AT COMPILE TIME.
    list_node->element = element; /// YOU SHOULD ALLOCATE DYNAMIC MEMORY WHEN THE ELEMENT NEEDS TO BE MODIFIED AND MAYBE INPUTTED BY THE USER (UNKOWN AT COMPILE TIME)

    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) {  //covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index) { // IMPORTANT TO HANDLE EACH SCENARIO (START, MIDDLE, END) INDIVIDUALLY
    dplist_node_t *reference = dpl_get_reference_at_index(list,index);
    if (reference != NULL) {
        if (reference->next == NULL && reference->prev == NULL) {
            list->head = NULL;
            free(reference);
        }
        else if (reference->next == NULL) {
            reference->prev->next = NULL;
            reference->prev = NULL;
            free(reference);
        }
        else if (reference->prev == NULL) {
            list->head = reference->next;
            reference->next->prev = list->head;
            free(reference);
        }
        else {
            reference->prev->next = reference->next;
            reference->next->prev = reference->prev;
            free(reference);
        }
        return list;
    }
    return NULL;
}

int dpl_size(dplist_t *list) {
    if (list == NULL) return -1; // NO list exists
    dplist_node_t *current_node = list->head;
    if (current_node == NULL) return 0; //List exists, no elements added
    if (current_node->next == NULL) return 1; // 1 element in list
    int count = 1;
    while (current_node->next != NULL) {
        current_node = current_node->next;
        count++;
    }
    return count; // This will always be 1 larger than the last index because we made it start from 1.
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    // If this function executes we know index is larger than 0 (check if statements in insert function)
    if  (list == NULL) return NULL;
    dplist_node_t *current_node = list->head;
    if (current_node == NULL) return NULL;
    if (current_node->next == NULL || index <= 0) return current_node;
    int count = 0;
    while (current_node->next != NULL && count <= index) {
        current_node = current_node->next;
        count++;
    }
    return current_node;
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {
    element_t p = NULL;
    dplist_node_t *reference = dpl_get_reference_at_index(list,index);
    if (reference != NULL) return reference->element;
    return p;
}

int dpl_get_index_of_element(dplist_t *list, element_t element) {

    if (list != NULL) {
        int size = dpl_size(list);
        for (int i = 0; i < size; i++) {
            char *current_element = dpl_get_element_at_index(list,i);
            if (element == current_element) {
                return i;
            }
        }
    }
    return -1;
}



