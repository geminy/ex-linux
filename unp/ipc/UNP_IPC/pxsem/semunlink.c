#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(argv[1]) == -1) {
        printf("sem_unlink error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
