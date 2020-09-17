#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (2 != argc) {
        printf("usage: %s <path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("OPEN_MAX = %ld, PIPE_BUF = %ld\n", sysconf(_SC_OPEN_MAX), pathconf(argv[1], _PC_PIPE_BUF));
    exit(EXIT_SUCCESS);
}
