//
// Created by martin on 11/16/25.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include <sys/types.h>

#include "sensor_db.h"
#include "logger.h"
#include "config.h"

int main () {

    // test inserting_value before opening file.
    FILE *f = NULL;
    sensor_id_t id = 1;
    sensor_value_t v = 0.001;
    sensor_ts_t ts = time(NULL);

    insert_sensor(f, id, v, ts); // Error on log file

    /**
     * Normal implementation
     */
    f = open_db("sensor_db.csv", true);

    sleep(1);

    insert_sensor(f, id, v, ts);

    id = 2;
    v = 0.002;
    ts = time(NULL);
    insert_sensor(f, id, v, ts);
    id = 3;
    v = 0.003;
    ts = time(NULL);
    insert_sensor(f, id, v, ts);
    sleep(5);
    insert_sensor(f, 4, v, ts);

    close_db(f);

    /**
    * End of normal implementation
    */

    //test inserting value after closing a file
    insert_sensor(f, 5, v, ts); // Error on log file

    return 0;
}