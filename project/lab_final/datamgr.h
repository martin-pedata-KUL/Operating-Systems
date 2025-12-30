//
// Created by martin on 12/13/25.
//

#ifndef DATAMGR_H_
#define DATAMGR_H_

#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "sbuffer.h"

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5
#endif

#ifndef SET_MAX_TEMP
#define SET_MAX_TEMP 20
#endif

#ifndef SET_MIN_TEMP
#define SET_MIN_TEMP 19
#endif

/*
 * Use ERROR_HANDLER() for handling memory allocation problems, invalid sensor IDs, non-existing files, etc.
 */
#define ERROR_HANDLER(condition, ...)    do {                       \
                      if (condition) {                              \
                        printf("\nError: in %s - function %s at line %d: %s\n", __FILE__, __func__, __LINE__, __VA_ARGS__); \
                        exit(EXIT_FAILURE);                         \
                      }                                             \
                    } while(0)

/**
 * Implementation-specific my_element_t struct and declaration of callback functions for this exercise ~ Martin
 */

typedef struct
{
    uint16_t room_id;
    sensor_id_t sensor_id;
    sensor_value_t readings[RUN_AVG_LENGTH];
    time_t timestamp;
} my_element_t;

void * element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void datamgr_sensor_mapping(FILE *fp_sensor_map);

void datamgr_read(sbuffer_t * sbuff);

void update_sensor_readings( my_element_t * element, sensor_ts_t sensor_ts, sensor_value_t sensor_value);

my_element_t * datamgr_get_sensor_node(sensor_id_t sensor_id);

void datamgr_free();

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id);

#endif  //DATAMGR_H_