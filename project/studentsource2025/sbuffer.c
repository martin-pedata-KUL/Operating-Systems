//
// Created by martin on 12/13/25.
//

#include "sbuffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "config.h"

pthread_mutex_t sbuff_mtx;
pthread_cond_t sbuff_n_empty;

int sbuffer_init(sbuffer_t **buffer) {
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    (*buffer)->connmgr_done = false;
    pthread_mutex_init(&sbuff_mtx, NULL);
    pthread_cond_init(&sbuff_n_empty, NULL);

    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    free(*buffer);
    *buffer = NULL;
    pthread_mutex_destroy(&sbuff_mtx);
    pthread_cond_destroy(&sbuff_n_empty);
    return SBUFFER_SUCCESS;
}

int sbuffer_get_head_content(sbuffer_t *sbuff, sensor_data_t * out) {
    pthread_mutex_lock(&sbuff_mtx);

    // Wait while there is no data
    while (sbuff->head != NULL && sbuff->head->read_by_datamgr) {
        pthread_cond_wait(&sbuff_n_empty, &sbuff_mtx);
    }

    //If buffer empty
    while (sbuff->head == NULL) {
        if (sbuff->connmgr_done) {
            pthread_mutex_unlock(&sbuff_mtx);
            return SBUFFER_TERMINATOR;
        }
        pthread_cond_wait(&sbuff_n_empty, &sbuff_mtx);
    }

    *out = sbuff->head->data;
    sbuff->head->read_by_datamgr = true;

    /* Wake storage manager */
    pthread_cond_signal(&sbuff_n_empty);

    pthread_mutex_unlock(&sbuff_mtx);
    return SBUFFER_SUCCESS;
}


int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *head_data) {
    pthread_mutex_lock(&sbuff_mtx);

    while (!buffer->connmgr_done && (buffer->head == NULL || !buffer->head->read_by_datamgr)) {
        pthread_cond_wait(&sbuff_n_empty, &sbuff_mtx);
    }

    if (buffer->head == NULL && buffer->connmgr_done) {
        // Buffer empty, no more data
        head_data->id = SBUFFER_TERMINATOR;
        head_data->value = 0;
        head_data->ts = 0;
        pthread_mutex_unlock(&sbuff_mtx);
        return SBUFFER_TERMINATOR;
    }

    // Remove head node
    sbuffer_node_t *dummy = buffer->head;

    *head_data = dummy->data;

    buffer->head = dummy->next;
    if (buffer->head == NULL)
        buffer->tail = NULL;

    pthread_cond_broadcast(&sbuff_n_empty);

    if (buffer->connmgr_done) {
        pthread_mutex_unlock(&sbuff_mtx);
        free(dummy);
        return SBUFFER_AFTERMATH;
    }
    pthread_mutex_unlock(&sbuff_mtx);
    free(dummy);
    return SBUFFER_SUCCESS;
}


int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->read_by_datamgr = false;
    dummy->read_by_strgmgr = false;
    dummy->next = NULL;

    pthread_mutex_lock(&sbuff_mtx);

    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    pthread_cond_signal(&sbuff_n_empty); // !

    pthread_mutex_unlock(&sbuff_mtx);

    return SBUFFER_SUCCESS;
}

int sbuffer_size(sbuffer_t *buffer) {
    if (buffer == NULL) return -1; // NO list exists

    pthread_mutex_lock(&sbuff_mtx);

    sbuffer_node_t *current_node = buffer->head;
    if (current_node == NULL) { //List exists, no elements added
        pthread_mutex_unlock(&sbuff_mtx);
        return 0;
    }

    if (current_node->next == NULL) { // 1 element in list
        pthread_mutex_unlock(&sbuff_mtx);
        return 1;
    }
    int count = 1;
    while (current_node->next != NULL) {
        current_node = current_node->next;
        count++;
    }

    pthread_mutex_unlock(&sbuff_mtx);

    return count;
}

void sbuff_connmgr_termination(sbuffer_t *sbuffer) {
    pthread_mutex_lock(&sbuff_mtx);
    if (!sbuffer->connmgr_done) {
        sbuffer->connmgr_done = true;
        pthread_cond_broadcast(&sbuff_n_empty);
    }
    pthread_mutex_unlock(&sbuff_mtx);
}