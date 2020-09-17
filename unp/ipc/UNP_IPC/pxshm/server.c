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
    int fd;
    struct shmstruct *ptr;
    if (3 != argc) {
        printf("usage: %s <shm_name> <sem_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    shm_unlink(argv[1]); // unlink if used, OK if failed
    // open shm
    if ((fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, FILE_MODE)) == -1) {
        printf("shm_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // set size
    if (ftruncate(fd, sizeof(struct shmstruct)) == -1) {
        printf("ftruncate error: %s\n", strerror(errno));
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
    sem_unlink(argv[2]); // unlink if used, OK if failed
    // open sem
    if ((sem = sem_open(argv[2], O_CREAT | O_EXCL, FILE_MODE, 1)) == SEM_FAILED) {
        printf("sem_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // close sem
    if (sem_close(sem) == -1) {
        printf("sem_close error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
