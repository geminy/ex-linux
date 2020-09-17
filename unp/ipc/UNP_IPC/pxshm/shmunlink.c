#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    if (2 != argc) {
        printf("usage: %s [-e] <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // sh_unlink
    if (shm_unlink(argv[1]) == -1) {
        printf("shm_unlink error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
