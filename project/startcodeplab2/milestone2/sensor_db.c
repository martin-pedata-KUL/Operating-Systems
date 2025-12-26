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
            msg = "existing csv file has been opened"; // Newline to ensure logger knows when the msg ends.
        }
        else {
            f = fopen(filename, "w");
            msg = "new csv file is being created";
        }
        write(fd[1],msg,strlen(msg)+1); // Pass the log msg to the child

        return f;
    }

    else { // Child: Our current child has no child, so pid = 0

        close(fd[WRITE_END]);

        char buffer[BUFFER_SIZE]; // Buffer of pipe
        char msgbuf[BUFFER_SIZE]; // Buffer containing unique and separate messages.
        int msglen = 0; //index of msgbuf.


        while (1) { // Blocks child in infinite loop, waiting for a read of a log message until the csv is closed

            int n = (int) read(fd[0],buffer,sizeof(buffer)); // read() does two things. Loads our pipe buffer, and returns the number of bytes read.

            for (int i = 0; i < n; i++) { //Load msg buffer. This logic ensures that msgbuf retains its content through multiple pipe reads, and only resets its indexing (msglen) upon seeing a '\0' in the buffer.
                msgbuf[msglen] = buffer[i];
                msglen++;
                if (buffer[i] == '\0') {
                    if (strcmp(msgbuf,"child exit") == 0) {
                        close(fd[READ_END]);
                        end_log_process();
                    }
                    write_to_log_process(msgbuf);
                    msglen = 0;
                    memset(msgbuf, 0, BUFFER_SIZE); // Complete message has been read. reset message buffer and its index.

                }
            }
        }
    }
    return f;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {

    if (f == NULL) { // File not opened
        printf("cannot read from closed file");
        return -1;
    }

    char tstr[30]; // Time buffer.
    strcpy(tstr, ctime(&ts)); // ctime() makes the time(NULL) result into a readable "Day-Month-Date-Hour-Year\n" format. We are copying into a buffer instead of taking string literal directly bcs we need to remove the newline at the end.
    tstr[strcspn(tstr, "\n")] = '\0'; // By replacing newline with null operator, we essentially remove the character (make the string end at the position where \n was)

    fprintf(f, "%u,", id); // fprintf() -> system call to write into OPENED file.
    fprintf(f, "%.10f,", value);
    fprintf(f, "%s\n", tstr);
    int flush = fflush(f); // fflush() sends the content of the buffer of the FILE stream (updated thus far with fprintf() ) to the OS at runtime. So that the OS can execute it (i.e. create the file at runtime). If it returns -1, the flush failed and file may not be updated.
    if (flush != 0) {
        char * msg = "failed to update csv with sensor data";
        write(fd[1],msg,strlen(msg)+1);
        return -1;
    }

    char * msg = "csv updated with sensor data";
    write(fd[1],msg,strlen(msg)+1);

    return 0;
}

int close_db(FILE * f) {
    fclose(f);

    char * msg = "csv closed successfully";
    write(fd[1],msg,strlen(msg)+1);

    msg = "child exit";
    write(fd[1],msg,strlen(msg)+1);

    close(fd[WRITE_END]); // Parent is done writing. Pipe closed on Parent's side.
    wait(NULL); // The parent waits for the child to end, to avoid an Orphan process.

    return 0;
}