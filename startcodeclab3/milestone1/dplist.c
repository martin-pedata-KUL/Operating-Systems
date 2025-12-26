#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"




/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {  // We pass a double pointer bcs we want to operate on the original list pointer stored in memory, and not on the content of the list pointer as would be the case if we passed a single pointer as a parameter.
    if (*list == NULL) {
        return;
    }
    if ((*list)->head != NULL) {
        for (int i = dpl_size(*list) - 1; i >= 0; i--) {
            *list = dpl_remove_at_index(*list, i, free_element); // Remove the last element of the list each time
        }
    }
    free(*list);
    *list = NULL; // Avoids dangling pointers: These are pointers that point to a memory address which has been freed.
                  //Memory manager, upon calling "free()" function, frees memory, but does not clear the inner content. So the programmer needs to clear it himself to avoid undefined behavior afterwards.
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
    return count; // This will always be 1 larger than the last index because we made "count" start from 1.

}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL; //If list doesn't exist, do nothing
    list_node = malloc(sizeof(dplist_node_t)); //Create node

    if (insert_copy == true && list->element_copy != NULL) { // I avoided using a ternary statement for clarity.

        list_node->element = list->element_copy(element); //Deep copy: creating new memory in heap (dynamic) with the same content as the element parameter, and making the node point to it.
    }
    else {
        list_node->element = element; // Making the node point to the element already found in memory.
    }

    if (list->head == NULL) { // If there's no nodes, add it as first (regardless of index
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;

    } else if (index <= 0) { // If index is negative or 0, add it as first node regardless of the specific value of index.
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;

    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);

        if (index < dpl_size(list)) {  // Normal case: Adding in the middle of the list
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // Adding at the end of the list
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;

        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {
    if (list == NULL) return NULL;
    dplist_node_t *reference = dpl_get_reference_at_index(list,index); // Does all the internal checking (i.e. returns first node if index <= 0,  last node if index bigger than size)
    if (reference != NULL) { // Check that node exists
        if (reference->next == NULL && reference->prev == NULL) { // First and only element of the list
            list->head = NULL;
        }
        else if (reference->next == NULL) { // Last element of the list
            reference->prev->next = NULL;
            reference->prev = NULL;
        }
        else if (reference->prev == NULL) { // First element of the list
            list->head = reference->next;
            reference->next->prev = NULL; // !! Do not assign it list->head as you did before. This is invalid because list->head is not a node !!!
        }
        else { //  In the middle of the list
            reference->prev->next = reference->next;
            reference->next->prev = reference->prev;
        }

        if (free_element == true) {
            list->element_free(&reference->element);
        }
        free(reference);

        return list;
    }
    return list;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    if (list == NULL || list->head == NULL) return NULL;

    dplist_node_t *current = list->head;
    int size = dpl_size(list);

    if (index <= 0) return current;
    if (index >= size - 1) {
        while (current->next != NULL)
            current = current->next;
        return current;
    }

    for (int i = 0; i < index; i++)
        current = current->next;

    return current;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {
    void * p = NULL;
    dplist_node_t * reference = dpl_get_reference_at_index(list,index);
    if (reference != NULL) return reference->element;
    return p;

}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    if (list != NULL) {
        if (list->element_compare == NULL) return -1;
        int size = dpl_size(list);
        for (int i = 0; i < size; i++) {
            void *current_element = dpl_get_element_at_index(list,i);
            if (list->element_compare(current_element,element) == 0 ) {
                return i;
            }
        }
    }
    return -1;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {
    if  (list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    if (reference == NULL) return NULL;
    int size = dpl_size(list);
    for (int i = 0; i < size; i++) {
        dplist_node_t *current_node = dpl_get_reference_at_index(list,i);
        if (list->element_compare(reference,current_node) == 0 ) {
            return current_node->element;
        }
    }
    return NULL;
}


