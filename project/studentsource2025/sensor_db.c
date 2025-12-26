//
// Created by martin on 12/13/25.
//
//
// Created by martin on 11/15/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include "sensor_db.h"

# define BUFFER_SIZE 256
# define READ_END 0
# define WRITE_END 1

int seq_nr; // Variable that retains its value across instantiations within one execution cycle
int fd[2]; // Create a length 2 array with fd[0] the read buffer and fd[1] the write buffer


// MAIN PROCESS: STORAGE MANAGER FUNCTIONS
FILE * open_db(char * filename) {

    FILE * f = NULL;

    if (pipe(fd) == -1) { // initialize the pipe
        return NULL;
    }

    // CREATION OF LOGGER PROCESS
    int pid = create_log_process(); // Fork operation. A child process, copy of the parent process, is created. This operation gives PID two concurrent values

    if (pid < 0) {
        return NULL;
    }
    if (pid > 0) { // Parent: Because the parent takes the pid of the child.
        close(fd[READ_END]); // YOU NEED TO CLOSE THE UNUSED CHANNEL, SO THAT THE USED ONE IS SPECIFIC TO THIS PROCESS (WRITE END SPECIFIC TO PARENT)
        f = fopen(filename, "w");
        return f;
    }

    else { // Child: Our current child has no child, so pid = 0

        close(fd[WRITE_END]);

        char buffer[BUFFER_SIZE]; // Buffer of pipe
        char msgbuf[BUFFER_SIZE]; // Buffer containing unique and separate messages.
        int msglen = 0; //index of msgbuf.

        FILE * log_f = fopen("gateway.log","a");
        while (1) { // LOGGER PROCESS STAYS HERE THE ENTIRE EXECUTION. (Consistently waits for a read of a log message until the csv is closed).

            int n = (int) read(fd[0],buffer,sizeof(buffer)); // read() does two things. Loads our pipe buffer, and returns the number of bytes read.

            for (int i = 0; i < n; i++) { //Load msg buffer. This logic ensures that msgbuf retains its content through multiple pipe reads, and only resets its indexing (msglen) upon seeing a '\0' in the buffer.
                msgbuf[msglen] = buffer[i];
                msglen++;
                if (buffer[i] == '\0') {
                    if (strcmp(msgbuf,"child exit") == 0) {
                        close(fd[READ_END]);
                        fclose(log_f);
                        end_log_process();
                    }
                    write_to_log_process(msgbuf,log_f);
                    msglen = 0;
                    memset(msgbuf, 0, BUFFER_SIZE); // Complete message has been read. reset message buffer and its index.
                }
            }
        }
    }
    return f;
}

int insert_sensor(FILE * f, sbuffer_t * sbuff) {

    if (f == NULL) { // File not opened
        return -1;
    }
    sensor_data_t node;
    while (1) {
        //Get first element of sbuffer and update node
        int res = sbuffer_remove(sbuff, &node);

        if (res == SBUFFER_TERMINATOR) {
            return res;
        }

        sensor_id_t head_id = node.id;
        sensor_value_t head_temp = node.value;
        sensor_ts_t head_ts = node.ts;

        // printf("***** REMOVED: Buffer Size: %d \n", sbuffer_size(sbuff));

        //WRITE
        fprintf(f, "%u,%.10f,%ld\n", head_id, head_temp, head_ts);
        int flush = fflush(f); // fflush() sends the content of the buffer of the FILE stream (updated thus far with fprintf() ) to the OS at runtime. So that the OS can execute it (i.e. create the file at runtime). If it returns -1, the flush failed and file may not be updated.
        if (flush != 0) {
            char * log_msg = "Failed to update csv with sensor data";
            write(fd[1],log_msg,strlen(log_msg)+1);
            return -1;
        }

        //LOG
        char log_msg[128];
        if (res == SBUFFER_AFTERMATH) {
            snprintf(log_msg, sizeof(log_msg),
                "Data insertion from sensor %d after connection closure succeeded",
                    head_id);
        }
        else {
            snprintf(log_msg, sizeof(log_msg),
                "Data insertion from sensor %d succeeded",
                    head_id);
        }
        write(fd[1],log_msg,strlen(log_msg)+1);
    }
    return 0;
}

int close_db(FILE * f) {
    static int closed = 0;
    if (closed) return 0;
    closed = 1;

    fclose(f);

    char * msg = "The data.csv file has been closed.";
    write(fd[1],msg,strlen(msg)+1);

    msg = "child exit";
    write(fd[1],msg,strlen(msg)+1);

    close(fd[WRITE_END]); // Parent is done writing. Pipe closed on Parent's side.
    wait(NULL); // The parent waits for the child to end, to avoid an Orphan process.

    return 0;
}

//LOGGER PROCESS: FUNCTIONS

int write_to_log_process(char *msg, FILE * log_f) {
    char log[BUFFER_SIZE];

    time_t ts = time(NULL);
    char tstr[30];
    strcpy(tstr, ctime(&ts));
    tstr[strcspn(tstr, "\n")] = '\0';

    sprintf(log,"%d - %s - %s\n",seq_nr,tstr,msg); // sprintf() formats a specific character array into what you want.

    fprintf(log_f, "%s", log);
    fflush(log_f);
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