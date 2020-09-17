/******************************
** A Client-Server Program
** Using fork and two pipes
** Client: father process, input a pathname
** Server: child process, open the pathname and write back to client
******************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE (100) // buffer size

// client/server, readfd for read, writefd for write
void client(int readfd, int writefd);
void server(int readfd, int writefd);

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

void client(int readfd, int writefd)
{
    size_t len;
    ssize_t n;
    char buff[MAXLINE] = { 0 };

    fgets(buff, MAXLINE, stdin); // using fgets to input from stdin
    len = strlen(buff);
    if ('\n' == buff[len - 1]) {
        --len; // delete newline from fgets
    }

    if (-1 == write(writefd, buff, len)) { // write pathname to pipe
        printf("write error: %s", strerror(errno));
    }

    while (0 < (n = read(readfd, buff, MAXLINE))) { // read data from pipe
        if (-1 == write(STDOUT_FILENO, buff, n)) {
            printf("write error: %s", strerror(errno)); // write data to stdout
        }
    }
}

void server(int readfd, int writefd)
{
    int fd;
    ssize_t n;
    char buff[MAXLINE + 1] = { 0 };

    if (-1 == (n = read(readfd, buff, MAXLINE))) { // read pathname from pipe
        printf("read error: %s", strerror(errno));
    }
    else if (0 == n) {
        perror("end-of-file while reading pathname");
    }
    buff[n] = '\0'; // null terminate

    if (-1 == (fd = open(buff, O_RDONLY))) {
        perror("open error");
        snprintf(buff + n, sizeof(buff) - n, ": can't oepn, %s\n", strerror(errno));
        n = strlen(buff);
        write(writefd, buff, n); // open failed and tell client
    }
    else {
        while (0 < (n = read(fd, buff, MAXLINE))) { // open succeeded and copy file to pipe
            if (-1 == write(writefd, buff, n)) {
                printf("write error: %s", strerror(errno));
            }
        }
        if (-1 == close(fd)) {
            printf("close fd error: %s", strerror(errno));
        }
    }
}
