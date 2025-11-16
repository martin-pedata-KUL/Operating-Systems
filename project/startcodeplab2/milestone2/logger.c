//
// Created by martin on 11/15/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"
#include "sensor_db.h"
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include "logger.h"
#include "sensor_db.h"

#define BUFFER_SIZE 256

static int seq_nr; // Variable that retains its value across instantiations within one execution cycle

int write_to_log_process(char *msg) {
    FILE * log_f = NULL;
    char log[BUFFER_SIZE];

    time_t ts = time(NULL);
    char tstr[30];
    strcpy(tstr, ctime(&ts));
    tstr[strcspn(tstr, "\n")] = '\0';

    sprintf(log,"%d - %s - %s\n",seq_nr,tstr,msg); // sprintf() formats a specific character array into what you want.

    log_f = fopen("gateway.log","a");

    fprintf(log_f, "%s", log);
    fflush(log_f);
    fclose(log_f);
    seq_nr++; // Increment event counter.
    return 0;
}

int create_log_process() {
    seq_nr = 0; //Define the first event's sequence number
    return fork();
}

int end_log_process() {
    exit(0); // Ends log (child) process
    return 0;
}