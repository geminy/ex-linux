#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

struct shmstruct // struct stored in shared memory
{
    int count;
};

sem_t *sem; // pointer to named semaphore

int main(int argc, char **argv)
{
    int fd, i, nloop;
    pid_t pid;
    struct shmstruct *ptr;
    if (4 != argc) {
        printf("usage: %s <shm_name> <sem_name> <#loops>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    nloop = atoi(argv[3]);
    // open shm
    if ((fd = shm_open(argv[1], O_RDWR, FILE_MODE)) == -1) {
        printf("shm_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // mmap shm
    if ((ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // close shm
    if (close(fd) == -1) {
        printf("close error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // open sem
    if ((sem = sem_open(argv[2], 0)) == SEM_FAILED) {
        printf("sem_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    pid = getpid();
    for (i = 0; i < nloop; ++i) {
        if (sem_wait(sem) == -1) {
            printf("sem_wait error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("pid %ld: %d\n", (long)pid, ptr->count++);
        if (sem_post(sem) == -1) {
            printf("sem_post error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
