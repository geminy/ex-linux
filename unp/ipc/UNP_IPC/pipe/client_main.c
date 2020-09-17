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

// client readfd for read, writefd for write
void client(int readfd, int writefd);

int main(int argc, char **argv)
{
    int readfd;
    int writefd;

    if (0 > (writefd = open(FIFO1, O_WRONLY, 0))) {
        printf("open %s error: %s", FIFO1, strerror(errno));
    }

    if (0 > (readfd = open(FIFO2, O_RDONLY, 0))) {
        printf("open %s error: %s", FIFO2, strerror(errno));
    }

    client(readfd, writefd); // father is a client

    close(readfd);
    close(writefd);

    unlink(FIFO1);
    unlink(FIFO2);

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
