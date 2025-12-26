//
// Created by martin on 11/13/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

int main() {
    int fd[2]; // Create a length 2 array with fd[0] the read buffer and fd[1] the write buffer

    if (pipe(fd) == -1) { // initialize the pipe
        printf("Pipe failed.\n");
        return 1;
    }

    int pid = fork(); // Fork operation. A child process, copy of the parent process, is created. This operation gives PID two concurrent values

    if (pid < 0) {
        printf("fork failed.\n");
        return 1;
    }
    if (pid > 0) { // Parent: Because the parent takes the pid of the child.
        close(fd[0]);
        char * str = "Hello Martin"; // reference to ASCII characters in heap
        write(fd[1],str,strlen(str)+1); // Passing the length of the string + 1 to the child buffer because the child will have a character array, who adds '\o' terminator
        printf("Parent sent: %s\n", str);
        wait(NULL);
        close(fd[1]);
    }

    else { // Child: Our current child has no child, so pid = 0
        close(fd[1]);
        char buffer[128]; // local read buffer. You choose how much it reads.

        read(fd[0],buffer,sizeof(buffer));

        for (int i = 0; i<strlen(buffer); i++) {
            buffer[i] = islower(buffer[i]) ? toupper(buffer[i]) : tolower(buffer[i]);
        }
        printf("Child received and modified: %s\n", buffer);
        close(fd[0]);
        return 0;
    }
    return 0;
}