//
// Created by martin on 12/13/25.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "lib/dplist.h"
#include "config.h"
#include "datamgr.h"
#include <string.h>
#include "sensor_db.h"

static dplist_t * list_sn = NULL;  // GLOBAL Variable: Can be accessed from anywhere (STATIC: only visible within this .c file)
extern int fd[2];

void *element_copy(void *element) {
    my_element_t *copy = malloc(sizeof(my_element_t));
    copy->sensor_id = ((my_element_t *)element)->sensor_id;
    copy->room_id = ((my_element_t *)element)->room_id;
    copy->timestamp = ((my_element_t *)element)->timestamp;
    for (int i = 0; i < RUN_AVG_LENGTH; i++) {
        copy->readings[i] = (((my_element_t *)element)->readings[i]);
    }
    return (void *)copy;
}

void element_free(void **element)
{
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((my_element_t *)x)->sensor_id < ((my_element_t *)y)->sensor_id) ? -1 : (((my_element_t *)x)->sensor_id == ((my_element_t *)y)->sensor_id) ? 0
                                                                                                                            : 1);
}

void datamgr_sensor_mapping(FILE *fp_sensor_map) {
    // Check if mapping file has been opened

    if (fp_sensor_map == NULL) {
        fprintf(stderr, "Error: sensor map file not opened.\n");
        return;
    }

    // Create double pointer list of sensor nodes from mapping file

    list_sn = dpl_create(element_copy, element_free, element_compare);
    uint16_t room_id, sensor_id;
    int i = 1;
    while (fscanf(fp_sensor_map, "%hu %hu", &room_id, &sensor_id) == 2) {
        my_element_t * current = malloc(sizeof(my_element_t));  // REQUIRED WHENEVER WE ARE CREATING A VARIABLE THAT MUST OUTLIVE THE SCOPE AND WE ARE NOT PROVIDED ELEMENTS TO START WITH (AS ARGUMENTS)
        current->room_id = room_id;
        current->sensor_id = sensor_id;
        for (int j = 0; j < RUN_AVG_LENGTH; j++)
            current->readings[j] = -1;
        current->timestamp = 0;
        dpl_insert_at_index(list_sn, current, i, false);
        i++;
    }
}

void datamgr_read(sbuffer_t * sbuff) {
    while (1) {
        sensor_data_t head_data; // !
        int res = sbuffer_get_head_content(sbuff, &head_data);

        if (res == SBUFFER_TERMINATOR) break;

        sensor_id_t head_id = head_data.id;

        my_element_t * element = datamgr_get_sensor_node(head_id);
        if (element == NULL) {
            char log_msg[128];
            snprintf(log_msg, sizeof(log_msg),
                    "Received sensor data with invalid sensor node ID %d",
                    head_id);
            write(fd[1],log_msg,strlen(log_msg)+1);
            continue;
        }

        update_sensor_readings(element, head_data.ts, head_data.value);

        sensor_value_t avg = datamgr_get_avg(head_id);

        char log_msg[128];
        if (avg > SET_MAX_TEMP) {
            snprintf(log_msg, sizeof(log_msg),
                     "Sensor node %d reports it's too hot (avg temp = %f)",
                     head_id, avg);
            write(fd[1],log_msg,strlen(log_msg)+1);
        }
        else if (avg < SET_MIN_TEMP) {
            snprintf(log_msg, sizeof(log_msg),
                    "Sensor node %d reports it's too cold (avg temp = %f)",
                    head_id, avg);
            write(fd[1],log_msg,strlen(log_msg)+1);
        }
    }
}

my_element_t * datamgr_get_sensor_node(sensor_id_t sensor_id) {
    for (int i = 0; i<dpl_size(list_sn); i++) {
        my_element_t *current = dpl_get_element_at_index(list_sn, i);
        if (current->sensor_id == sensor_id) {
            return current;
        }
    }
    return NULL;
}

void update_sensor_readings (my_element_t * node, sensor_ts_t sensor_ts, sensor_value_t sensor_value) {

    // Update timestamp
    node->timestamp = sensor_ts;

    // Update last measurements
    for (int i = 0; i<RUN_AVG_LENGTH-1; i++) {
        node->readings[i] = node->readings[i+1];
    }
    node->readings[RUN_AVG_LENGTH-1] = sensor_value;

}

void datamgr_free() {
    dpl_free(&list_sn,true);
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id) {

    my_element_t * element = datamgr_get_sensor_node(sensor_id);
    ERROR_HANDLER(element == NULL, "Invalid sensor id");

    int non_valid_readings = 0;
    sensor_value_t sum = 0;
    for (int i = 0; i<RUN_AVG_LENGTH; i++) {
        if (element->readings[i] == -1) { // If not enough readings have been recorded (aka one of the values is still -1) then return the average as 0.
            non_valid_readings++;
            continue;
        }
        sum += element->readings[i];
    }

    sensor_value_t avg = sum/(RUN_AVG_LENGTH-non_valid_readings);

    return avg;
}
