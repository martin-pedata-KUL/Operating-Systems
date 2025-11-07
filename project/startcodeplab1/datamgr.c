#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>


#include "lib/dplist.h"
#include "config.h"
#include "datamgr.h"

static dplist_t * list_sn = NULL;  // GLOBAL Variable: Can be accessed from anywhere (STATIC: only visible within this .c file)

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


void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data) {

    // Check if mapping file has been opened

    if (fp_sensor_map == NULL) {
        fprintf(stderr, "Error: sensor map file not opened.\n");
        return;
    }

    // Create double pointer list from mapping file

    list_sn = dpl_create(element_copy, element_free, element_compare);
    uint16_t room_id, sensor_id;
    int i = 1;
    while (fscanf(fp_sensor_map, "%hu %hu", &room_id, &sensor_id) == 2) {
        printf("Room ID: %hu, Sensor ID: %hu\n", room_id, sensor_id);
        my_element_t * current = malloc(sizeof(my_element_t));  // REQUIRED WHENEVER WE ARE CREATING A VARIABLE THAT MUST OUTLIVE THE SCOPE AND WE ARE NOT PROVIDED ELEMENTS TO START WITH (AS ARGUMENTS)
        current->room_id = room_id;
        current->sensor_id = sensor_id;
        for (int j = 0; j < RUN_AVG_LENGTH; j++)
            current->readings[j] = -1;
        current->timestamp = 0;
        dpl_insert_at_index(list_sn, current, i, false);
        i++;
    }

    // Read binary file with sensor data

    while (1) {

        sensor_id_t sensor_id_rb;
        sensor_ts_t sensor_ts_rb;
        sensor_value_t sensor_temp_rb;

        // Read sensor_id (2 bytes)
        if (fread(&sensor_id_rb, sizeof(uint16_t), 1, fp_sensor_data) != 1) // In this line of code, two functions are executed. Firstly, the allocation of the next bytes to the local variable sensor_id_rb. Secondly, the check that there are indeed successive bytes to read from.
            break; // reached EOF or read error

        // Read temperature (8 bytes)
        if (fread(&sensor_temp_rb, sizeof(double), 1, fp_sensor_data) != 1)
            break; // read error (no corresponding binary value found in file)

        // Read timestamp (sizeof(time_t) bytes)
        if (fread(&sensor_ts_rb, sizeof(time_t), 1, fp_sensor_data) != 1)
            break; // idem

        my_element_t * element = datamgr_get_sensor_node(sensor_id_rb);
        if (element == NULL) {
            fprintf(stderr, "Error: No such sensor id in the list.\n");
        }

        update_sensor_readings(element, sensor_ts_rb, sensor_temp_rb);

        sensor_value_t avg = datamgr_get_avg(sensor_id_rb);

        if (avg > SET_MAX_TEMP) {
            fprintf(stderr, "Error: Average above the set max temp of %d.\n", SET_MAX_TEMP);
        }
        else if (avg < SET_MIN_TEMP) {
            fprintf(stderr, "Error: Average below the set min temp of %d.\n", SET_MIN_TEMP);
        }
    }



}

my_element_t * datamgr_get_sensor_node(sensor_id_t sensor_id) {
    for (int i = 0; i<dpl_get_sensors(); i++) {
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

uint16_t datamgr_get_room_id(sensor_id_t sensor_id)  {
    uint16_t id = 0;
    for (int i =  0; i < dpl_get_sensors(); i++) {
        my_element_t * element = dpl_get_element_at_index(list_sn,i);
        if (sensor_id == element->sensor_id) {
            return element->room_id;
        }
    }
    ERROR_HANDLER(id == 0, "Invalid sensor id");
    return id;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id) {

    my_element_t * element = datamgr_get_sensor_node(sensor_id);
    ERROR_HANDLER(element == NULL, "Invalid sensor id");

    sensor_value_t sum = 0;
    for (int i = 0; i<RUN_AVG_LENGTH; i++) {
        if (element->readings[i] == -1) { // If not enough readings have been recorded (aka one of the values is still -1) then return the average as 0.
            return 0;
        }
        sum += element->readings[i];
    }

    sensor_value_t avg = sum/RUN_AVG_LENGTH;

    return avg;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id) {
    int ts = -1;
    for (int i =  0; i < dpl_get_sensors(); i++) {
        my_element_t * element = dpl_get_element_at_index(list_sn,i);
        if (sensor_id == element->sensor_id) {
            return element->timestamp;
        }
    }
    ERROR_HANDLER(ts == -1, "Invalid sensor id");
    return -1;
}

int dpl_get_sensors() {
    return dpl_size(list_sn);
}


