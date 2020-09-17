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
    int c, flags;
    mqd_t mqd;
    ssize_t n;
    unsigned prio;
    void *buff;
    struct mq_attr attr;
    flags = O_RDWR;
    while (-1 != (c = getopt(argc, argv, "n"))) {
        switch (c) {
        case 'n':
            flags |= O_NONBLOCK;
            break;
        }
    }
    if (optind != argc - 1) {
        printf("usage: %s [-n] <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    mqd = mq_open(argv[optind], flags);
    if (-1 == mqd) {
        printf("mq_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (-1 == mq_getattr(mqd, &attr)) {
        printf("mq_getattr error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    buff = malloc(attr.mq_msgsize);
    if (NULL == buff) {
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }
    if (-1 == (n = mq_receive(mqd, buff, attr.mq_msgsize, &prio))) {
        printf("mq_receive error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("read %ld bytes, priority = %u\n", (long)n, prio);
    exit(EXIT_SUCCESS);
}
