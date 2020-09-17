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
    int readfifo, writefifo;
    char *ptr, buff[MAXLINE], fifoname[MAXLINE];
    pid_t pid;
    ssize_t n;
    size_t len;

    pid = getpid();
    snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long)pid);
    // create FIFO with our PID as part of name
    if ((0 > mkfifo(fifoname, FILE_MODE)) && (EEXIST != errno)) {
        printf("mkfifo %s error: %s", fifoname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    snprintf(buff, sizeof(buff), "%ld ", (long)pid);
    len = strlen(buff);
    ptr = buff + len;   
    fgets(ptr, MAXLINE - len, stdin); // read pathname
    len = strlen(buff); // fgets guarantees null byte at end

    // open FIFO to server and write PID and pathname to FIFO
    if (0 > (writefifo = open(SERV_FIFO, O_WRONLY, 0))) {
        printf("open %s error: %s", SERV_FIFO, strerror(errno));
        unlink(fifoname);
        exit(EXIT_FAILURE);
    }
    write(writefifo, buff, len);

    // now open our FIFO, blocks untill server opens for writing
    if (0 > (readfifo = open(fifoname, O_RDONLY, 0))) {
        printf("open %s error: %s", fifoname, strerror(errno));
        unlink(fifoname);
        exit(EXIT_FAILURE);
    }

    while (0 < (n = read(readfifo, buff, MAXLINE))) {
        write(STDOUT_FILENO, buff, n);
    }

    close(readfifo);
    unlink(fifoname);

    exit(EXIT_SUCCESS);
}
