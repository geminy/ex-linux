#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXLINE (100) // buffer size

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

// server, readfd for read, writefd for write
void server(int readfd, int writefd);

int main(int argc, char **argv)
{
    int readfd;
    int writefd;

    if ((0 > mkfifo(FIFO1, FILE_MODE)) && (EEXIST != errno)) { // make fifo, errno can be EEXIST
        printf("mkfifo %s error: %s", FIFO1, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((0 > mkfifo(FIFO2, FILE_MODE)) && (EEXIST != errno)) { // make fifo, errno can be EEXIST
        printf("mkfifo %s error: %s", FIFO2, strerror(errno));
        unlink(FIFO1); // remove unuesd fifo
        exit(EXIT_FAILURE);
    }

    if (0 > (readfd = open(FIFO1, O_RDONLY, 0))) {
        printf("open %s error: %s", FIFO1, strerror(errno));
    }

    if (0 > (writefd = open(FIFO2, O_WRONLY, 0))) {
        printf("open %s error: %s", FIFO2, strerror(errno));
    }

    server(readfd, writefd); // child is a server

    exit(EXIT_SUCCESS);
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
