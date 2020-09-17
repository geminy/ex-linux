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
    int fd, i, nloop, zero = 0;
    int *ptr;
    sem_t *sem;
    if (3 != argc) {
        printf("usage: %s <pathname> <#loops>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    nloop = atoi(argv[2]);
    // open file
    if ((fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE)) == -1) {
        printf("open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // initialize fd to 0
    if (write(fd, &zero, sizeof(int)) == -1) {
        printf("write error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // map into memory
    if ((ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // close file
    if (close(fd) == -1) {
        printf("close error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // create semaphore and initialize to 1
    if ((sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, FILE_MODE, 1)) == SEM_FAILED) {
        printf("sem_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // unlink semaphore
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
    for (i = 0; i < nloop; ++i) { // parent
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
