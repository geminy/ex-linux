#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv)
{
    int c, flags;
    sem_t *sem;
    unsigned int value;
    flags = O_RDWR | O_CREAT;
    value = 1;
    while ((c = getopt(argc, argv, "ei:")) != -1) {
        switch (c) {
        case 'e':
            flags |= O_EXCL;
            break;
        case 'i':
            value = atoi(optarg);
            break;
        }
    }
    if (optind != argc - 1) {
        printf("usage: %s [-e] [-i initvalue] <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    ;
    if ((sem = sem_open(argv[optind], flags, FILE_MODE, value)) == SEM_FAILED) {
        printf("sem_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (sem_close(sem) == -1) {
        printf("sem_close error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
