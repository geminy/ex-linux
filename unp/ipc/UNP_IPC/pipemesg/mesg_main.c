#include "pipemesg.h"

#define MAXLINE (100) // buffer size
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv)
{
    int pipe1[2];
    int pipe2[2];
    pid_t childfd;

    if (-1 == pipe(pipe1) || -1 == pipe(pipe2)) { // create two pipes
        printf("create pipe error: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (0 == (childfd = fork())) { // child
        // using pipe1[0] for read and pipe2[1] for write
        // so close unused pipe1[1] and pipe2[0]
        if (-1 == close(pipe1[1]) || -1 == close(pipe2[0])) {
            printf("close pipe error: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        server(pipe1[0], pipe2[1]); // child is a server
        exit(EXIT_SUCCESS);
    }

    // using pipe2[0] for read and pipe1[1] for write
    // so close unused pipe1[0] and pipe2[1]
    if (-1 == close(pipe1[0]) || -1 == close(pipe2[1])) {
        printf("close pipe error: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    client(pipe2[0], pipe1[1]); // father is a client

    if (-1 == waitpid(childfd, NULL, 0)) { // wait for child to terminate
        perror("waitpid error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
