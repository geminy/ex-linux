#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv)
{
    int i, fd;
    struct stat stat;
    unsigned char *ptr;
    if (2 != argc) {
        printf("usage: %s <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // sh_open
    if ((fd = shm_open(argv[1], O_RDWR, FILE_MODE)) == -1) {
        printf("shm_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // fstat
    if (fstat(fd, &stat) == -1) {
        printf("fstat error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // mmap
    if ((ptr = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // close
    if (close(fd) == -1) {
        printf("close error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < stat.st_size; ++i) {
        *ptr++ = i % 256;
    }
    exit(EXIT_SUCCESS);
}
