#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>

int main(int argc, char **argv)
{
    sem_t *sem;
    int value;
    if (argc != 2) {
        printf("usage: %s <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if ((sem = sem_open(argv[1], 0)) == SEM_FAILED) {
        printf("sem_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (sem_post(sem) == -1) {
        printf("sem_post error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (sem_getvalue(sem, &value) == -1) {
        printf("sem_getvalue error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("value = %d\n", value);
    exit(EXIT_SUCCESS);
}
