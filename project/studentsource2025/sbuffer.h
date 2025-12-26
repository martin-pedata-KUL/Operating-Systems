//
// Created by martin on 12/13/25.
//

#ifndef STUDENTSOURCE2025_SBUFFER_H
#define STUDENTSOURCE2025_SBUFFER_H
#include <stdbool.h>

#include "config.h"

#define SBUFFER_FAILURE -1
#define SBUFFER_SUCCESS 0
#define SBUFFER_AFTERMATH 1
#define SBUFFER_NO_DATA -3
#define SBUFFER_TERMINATOR -2

typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
    bool read_by_datamgr;
    bool read_by_strgmgr;
} sbuffer_node_t;

typedef struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    bool connmgr_done;
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
} sbuffer_t;

int sbuffer_init(sbuffer_t **buffer);

int sbuffer_size(sbuffer_t *buffer);

int sbuffer_get_head_content(sbuffer_t * sbuff, sensor_data_t * out);

int sbuffer_free(sbuffer_t **buffer);

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t * head_data);

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data);

void sbuff_connmgr_termination(sbuffer_t * sbuff);
#endif //