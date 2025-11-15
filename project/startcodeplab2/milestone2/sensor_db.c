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
#include "sensor_db.h"
#include "logger.h"

# define BUFFER_SIZE 256
# define READ_END 0
# define WRITE_END 1


int fd[2]; // Create a length 2 array with fd[0] the read buffer and fd[1] the write buffer

//void update_buff_till_nl(char * str, char * msg);

FILE * open_db(char * filename, bool append) {

    FILE * f = NULL;

    if (pipe(fd) == -1) { // initialize the pipe
        printf("Pipe failed.\n");
        return NULL;
    }

    int pid = create_log_process();; // Fork operation. A child process, copy of the parent process, is created. This operation gives PID two concurrent values

    if (pid < 0) {
        printf("fork failed.\n");
        return NULL;
    }
    if (pid > 0) { // Parent: Because the parent takes the pid of the child.

        close(fd[READ_END]); // YOU NEED TO CLOSE THE UNUSED CHANNEL, SO THAT THE USED ONE IS SPECIFIC TO THIS PROCESS (WRITE END SPECIFIC TO PARENT)
        char * msg;
        if (append) {
            f = fopen(filename, "a");
            msg = "existing csv file has been opened\n"; // Newline to ensure logger knows when the msg ends.
        }
        else {
            f = fopen(filename, "w");
            msg = "new csv file is being created\n";
        }
        write(fd[1],msg,strlen(msg)+1); // Pass the log msg to the child

        return f;
    }

    else { // Child: Our current child has no child, so pid = 0
        close(fd[WRITE_END]);

        while (1) { // Blocks child in infinite loop, waiting for a read of a log message until the csv is closed
            char buffer[BUFFER_SIZE];
            read(fd[0],buffer,sizeof(buffer));

            // char msg[BUFFER_SIZE];
            //
            // update_buff_till_nl(buffer,msg); // Update msg so that its last character excluding terminator is \n. This will be the actual log message with extra buffer space removed.

            if (strcmp(buffer,"child exit") == 0) {
                break;
            }

            write_to_log_process(buffer);

        }
        close(fd[READ_END]); // Child process is done reading. pipe on Child's side is completely closed. Ready to be ended
        end_log_process();
    }
    return f;
}

// void update_buff_till_nl(char buffer[], char out[]) {
//     int i = 0;
//     while (buffer[i] != '\n' && buffer[i] != '\0') {
//         out[i] = buffer[i];
//         i++;
//     }
//     out[i] = '\0';
// }

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {

    if (f == NULL) { // File not opened
        return -1;
    }

    fprintf(f, "%u,", id); // fprintf() -> system call to write into OPENED file.
    fprintf(f, "%.10f,", value);
    fprintf(f, "%ld\n", ts);
    int flush = fflush(f); // fflush() sends the content of the buffer of the FILE stream (updated thus far with fprintf() ) to the OS at runtime. So that the OS can execute it (i.e. create the file at runtime). If it returns -1, the flush failed and file may not be updated.
    if (flush != 0) {
        char * msg = "failed to update csv with sensor data\n";
        write(fd[1],msg,strlen(msg)+1);
        return -1;
    }

    char * msg = "csv updated with sensor data\n";
    write(fd[1],msg,strlen(msg)+1);

    return 0;
}

int close_db(FILE * f) {
    fclose(f);

    char * msg = "csv closed successfully\n";
    write(fd[1],msg,strlen(msg)+1);

    sleep(2);

    msg = "child exit\n";
    write(fd[1],msg,strlen(msg)+1);

    close(fd[WRITE_END]); // Parent is done writing. Pipe closed on Parent's side.
    wait(NULL); // The parent waits for the child to end, so as to avoid an Orphan process.

    return 0;
}