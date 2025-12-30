//
// Created by martin on 12/13/25.
//

#ifndef STUDENTSOURCE2025_SENSOR_DB_H
#define STUDENTSOURCE2025_SENSOR_DB_H

#endif //STUDENTSOURCE2025_SENSOR_DB_H

#include <stdio.h>
#include "sbuffer.h"

FILE * open_db(char * filename);
int insert_sensor(FILE * f, sbuffer_t * sbuff);
int close_db(FILE * f);
int write_to_log_process(char *msg, FILE * log_f);
int create_log_process();
void end_log_process();