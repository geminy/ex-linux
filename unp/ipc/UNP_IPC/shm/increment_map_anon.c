#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define SEM_NAME "/semtest"

int main(int argc, char **argv)
{
    int i, nloop;
    int *ptr;
    sem_t *sem;
    if (2 != argc) {
        printf("usage: %s  <#loops>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    nloop = atoi(argv[1]);
    // anonymous mmap
    if ((ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // create, initialize, and unlink semaphore
    if ((sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, FILE_MODE, 1)) == SEM_FAILED) {
        printf("sem_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(SEM_NAME) == -1) {
        printf("sem_unlink error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // stdout is unbuffered
    setbuf(stdout, NULL);
    if (fork() == 0) { // child
        for (i = 0; i < nloop; ++i) {
            if (sem_wait(sem) == -1) {
                printf("sem_wait child: error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            printf("child: %d\n", (*ptr)++);
            if (sem_post(sem) == -1) {
                printf("sem_post child error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_SUCCESS);
    }
    for (i = 0; i < nloop; ++i) { //parent
        if (sem_wait(sem) == -1) {
            printf("sem_wait parent error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("parent: %d\n", (*ptr)++);
        if (sem_post(sem) == -1) {
            printf("sem_post parent error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}

