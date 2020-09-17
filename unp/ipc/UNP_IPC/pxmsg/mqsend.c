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
    mqd_t mqd;
    void *ptr;
    size_t len;
    unsigned prio;
    if (4 != argc) {
        printf("usage: %s <name>  <bytes> <priority>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    len = atoi(argv[2]);
    prio = atoi(argv[3]);
    mqd = mq_open(argv[1], O_WRONLY);
    if (-1 == mqd) {
        printf("mq_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    ptr = calloc(len, sizeof(char));
    if (NULL == ptr) {
        printf("calloc error\n");
        exit(EXIT_FAILURE);
    }
    if (-1 == mq_send(mqd, ptr, len, prio)) {
        printf("mq_send error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
