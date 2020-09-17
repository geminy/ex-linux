#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define max(a, b) ((a) > (b) ? (a) : (b))

int main(int argc, char **argv)
{
    int fd, i;
    char *ptr;
    size_t filesize, mmapsize, pagesize;
    if (4 != argc) {
        printf("usage: %s <pathname> <filesize> <mmapsize>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    filesize = atoi(argv[2]);
    mmapsize = atoi(argv[3]);
    // open file, create or truncate
    if ((fd = open(argv[1], O_RDWR | O_TRUNC | O_CREAT, FILE_MODE)) == -1) {
        printf("open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // set offset
    if (lseek(fd, filesize - 1, SEEK_SET) == -1) {
        printf("lseek error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // write 1 byte
    if (write(fd, "", 1) == -1) {
        printf("write error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // mmap
    if ((ptr = mmap(NULL, mmapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // close fd
    if (close(fd) == -1) {
        printf("close error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // get pagesize
    if ((pagesize = sysconf(_SC_PAGESIZE)) == -1) {
        printf("sysconf error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("page size = %ld, file size = %d, mmap size = %d\n", (long)pagesize, filesize, mmapsize);
    for (i = 0; i < max(filesize, mmapsize); i += pagesize) {
        printf("ptr[%d] = %d\n", i, ptr[i]);
        ptr[i] = 1;
        printf("ptr[%d] = %d\n", i + pagesize - 1, ptr[i + pagesize - 1]);
        ptr[i + pagesize - 1] = 1;
    }
    printf("ptr[%d] = %d\n", i, ptr[i]);
    printf("done\n");
    exit(EXIT_SUCCESS);
}



