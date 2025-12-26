
// Created by martin on 12/13/25.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "connmgr.h"
#include "datamgr.h"
#include "sbuffer.h"
#include "sensor_db.h"

extern int fd[2];
sbuffer_t *sbuffer = NULL;

FILE *f_csv = NULL; // CSV is stored in a global variable so it can be manipulated by the storage manager
FILE * f_log = NULL; // Used in Storage Manager
FILE *room_sensor_f = NULL; // Used by Data manager

void * connection_mgr (void * conn_block_prmtr) {
    conn_block_t * conn_block = (conn_block_t *) conn_block_prmtr;
    sbuffer_t * sbuff = conn_block->buffer;
    int max_conn = conn_block->max_conn;
    int port_nr = conn_block->port_nr;
    if (sbuff == NULL) return NULL;

    connection(sbuff, max_conn, port_nr); // Initialization of connection manager (server -> listens for connections) and client threads ( send data from sensor_node.c to shared buffer through TCP). Listens to, and records data of, new connections

    return NULL;
}

void * data_mgr (void * sbuffer) {
    sbuffer_t * sbuff = (sbuffer_t *) sbuffer;
    if (sbuff == NULL) return NULL;

    datamgr_sensor_mapping(room_sensor_f); // Create dpl containing data for each sensor from the room-sensor mapping file.
    datamgr_read(sbuff); // Start permanently reading the head of the list, calculate the average, and log the info.
    datamgr_free();

    return NULL;
}

void * storage_mgr (void * sbuffer) {
    sbuffer_t * sbuff = (sbuffer_t *) sbuffer;
    if (sbuff == NULL) return NULL;

    insert_sensor(f_csv, sbuff); // Permanently reads the first element of buffer, pops it and writes it into .csv

    return NULL;
}

int main(int argc, char *argv[]) {

    if(argc < 3) {
        return -1;
    }

    int port_nr = atoi(argv[1]);
    int max_conn = atoi(argv[2]);

    pthread_t conn_thread, data_thread, strg_thread;

    // Initialize shared buffer as well as mutex and condition variables
    sbuffer_init(&sbuffer);

    // Initialize connection manager struct
    conn_block_t conn_block;
    conn_block.buffer = sbuffer;
    conn_block.max_conn = max_conn;
    conn_block.port_nr = port_nr;

    // Create empty .csv and .log file and open room_sensor.map in read-only mode.

    f_csv = open_db("data.csv");  // Creates pipe + logger child process
    f_log = fopen("gateway.log","w"); // Creates logger output for this execution. Older logs are overwritten
    room_sensor_f = fopen("room_sensor.map", "r"); // Reads room-sensor mapping
    char * log_msg;

    if (f_csv == NULL) {
        log_msg = "could not open f_csv";
        write(fd[1],log_msg,strlen(log_msg)+1);
        return -1;
    }
    else {
        log_msg = "A new data.csv file has been created";
        write(fd[1],log_msg,strlen(log_msg)+1);
    }

    if (f_log == NULL) {
        log_msg = "could not open f_log";
        write(fd[1],log_msg,strlen(log_msg)+1);
        return -1;
    }
    if (room_sensor_f == NULL) {
        log_msg = "could not open room_sensor_f";
        write(fd[1],log_msg,strlen(log_msg)+1);
        return -1;
    }


    // Create threads
    pthread_create(&conn_thread, NULL, connection_mgr, &conn_block);
    pthread_create(&data_thread, NULL, data_mgr, sbuffer);
    pthread_create(&strg_thread, NULL, storage_mgr, sbuffer);

    // Wait for threads to finish
    pthread_join(conn_thread, NULL);
    pthread_join(data_thread, NULL);
    pthread_join(strg_thread, NULL);

    // Free heap memory anc close files safely
    sbuffer_free(&sbuffer);
    fclose(room_sensor_f);
    fclose(f_log);
    close_db(f_csv);

    return 0;
}

