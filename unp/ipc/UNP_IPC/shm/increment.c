#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int g_count = 0;

int main(int argc, char **argv)
{
    int i, nloop;
    if (2 != argc) {
        printf("usage: %s <#loops>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    nloop = atoi(argv[1]);
    setbuf(stdout, NULL);
    if (fork() == 0) { // child
        for (i = 0; i < nloop; ++i) {
            printf("child: %d\n", g_count++);
        }
        exit(EXIT_SUCCESS);
    }
    for (i = 0; i < nloop; ++i) { // parent
        printf("parent: %d\n", g_count++);
    }
    exit(EXIT_SUCCESS);
}
