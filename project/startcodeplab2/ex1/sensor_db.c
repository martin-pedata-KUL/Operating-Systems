//
// Created by martin on 11/13/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"
#include "sensor_db.h"



FILE * open_db(char * filename, bool append) {
    FILE * f = NULL;

	f = append ? fopen(filename, "a") : fopen(filename, "w");

    return f;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    fprintf(f, "%u,", id); // fprintf() -> system call to write into OPENED file.
    fprintf(f, "%.10f,", value);
    fprintf(f, "%ld\n", ts);
    fflush(f); // Ensures f is updated regardless of whether a system error occurs before execution of main() ends
    return 0;
}

int close_db(FILE * f) {
    fclose(f);
    return 0;
}
