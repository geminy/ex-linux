#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s  <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (-1 == mq_unlink(argv[1])) {
        printf("mq_unlink error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
