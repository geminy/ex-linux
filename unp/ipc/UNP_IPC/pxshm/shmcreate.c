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
    int c, fd, flags;
    char *ptr;
    off_t length;
    flags = O_RDWR | O_CREAT;
    while ((c = getopt(argc, argv, "e")) != -1) {
        switch (c) {
        case 'e':
            flags |= O_EXCL;
            break;
        }
    }
    if (argc - 2 != optind) {
        printf("usage: %s [-e] <name> <length>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    length = atol(argv[optind + 1]);
    // sh_open
    if ((fd = shm_open(argv[optind], flags, FILE_MODE)) == -1) {
        printf("shm_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // ftruncate
    if (ftruncate(fd, length) == -1) {
        printf("ftruncate error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // mmap
    if ((ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
