#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXLINE (100) // buffer size
#define SERV_FIFO "/tmp/fifo.serv" // server's well-known FIFO
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) // (FILE_MODE & ~umask)

int main(int argc, char **argv)
{
    int readfifo, writefifo, dummyfd, fd;
    char *ptr, buff[MAXLINE], fifoname[MAXLINE];
    pid_t pid;
    ssize_t n;

    // create server's well-known FIFO, OK if already exists
    if ((0 > mkfifo(SERV_FIFO, FILE_MODE)) && (EEXIST != errno)) {
        printf("mkfifo %s error: %s", SERV_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // open server's well-known FIFO for reading and writing
    if (0 > (readfifo = open(SERV_FIFO, O_RDONLY, 0))) {
        printf("open %s error: %s", SERV_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (0 > (dummyfd = open(SERV_FIFO, O_WRONLY, 0))) {  // never used, blocks at read insteand of re-open
        printf("open %s error: %s", SERV_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (0 < (n = read(readfifo, buff, MAXLINE))) {
        if ('\n' == buff[n - 1]) {
            --n; // delete newline from read
        }
        buff[n] = '\0'; // null terminate pathname

        if (NULL == (ptr = strchr(buff, ' '))) {
            printf("** request: %s, error input from client", buff);
            continue;
        }

        *ptr++ = 0; // null termiate PID, ptr = pathname
        pid = atol(buff);
        snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long)pid);

        if (0 > (writefifo = open(fifoname, O_WRONLY, 0))) {
            printf("open %s error: %s", fifoname, strerror(errno));
            continue;
        }

        if (0 > (fd = open(ptr, O_RDONLY, 0))) { // error: must tell client
            snprintf(buff + n, sizeof(buff) - n, ": can't open, %s\n", strerror(errno));
            n = strlen(ptr);
            write(writefifo, ptr, n);
            close(writefifo);
        }
        else { // open succeded: copy file to FIFO
            while (0 < (n = read(fd, buff, MAXLINE))) {
                write(writefifo, buff, n);
            }
            close(fd);
            close(writefifo);
        }
    }

//    unlink(readfifo); // no need remove server FIFO

    exit(EXIT_SUCCESS);
}
