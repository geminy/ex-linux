#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

struct shared
{
    sem_t sem;
    int count;
} shared;

int main(int argc, char **argv)
{
    int fd, i, nloop;
    struct shared *ptr;
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
    if (write(fd, &shared, sizeof(struct shared)) == -1) {
        printf("write error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // map into memory
    if ((ptr = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // close file
    if (close(fd) == -1) {
        printf("close error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // initialize semaphore that is shared between processes
    if (sem_init(&ptr->sem, 1, 1) == -1) {
        printf("sem_init error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // stdout is unbuffered
    setbuf(stdout, NULL);
    if (fork() == 0) { // child
        for (i = 0; i < nloop; ++i) {
            if (sem_wait(&ptr->sem) == -1) {
                printf("sem_wait child: error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            printf("child: %d\n", ptr->count++);
            if (sem_post(&ptr->sem) == -1) {
                printf("sem_post child error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_SUCCESS);
    }
    for (i = 0; i < nloop; ++i) { // parent
        if (sem_wait(&ptr->sem) == -1) {
            printf("sem_wait parent error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("parent: %d\n", ptr->count++);
        if (sem_post(&ptr->sem) == -1) {
            printf("sem_post parent error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
