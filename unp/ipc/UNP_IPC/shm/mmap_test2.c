#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FILE_NAME "mmap_test"
#define MMAP_SIZE (32768)

int main(int argc, char **argv)
{
    int fd, i;
    char *ptr;
    // open file, create or truncate
    if ((fd = open(FILE_NAME, O_RDWR | O_TRUNC | O_CREAT, FILE_MODE)) == -1) {
        printf("open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // mmap
    if ((ptr = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (i = 0; i <= MMAP_SIZE; i += 4096) {
        printf("setting file size to %d\n", i);
        ftruncate(fd, i);
        printf("ptr[%d] = %d\n", i - 1, ptr[i - 1]);
    }
    // close fd
    if (close(fd) == -1) {
        printf("close error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("done\n");
    exit(EXIT_SUCCESS);
}




