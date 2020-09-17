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
    struct mq_attr attr;
    if (argc != 2) {
        printf("usage: %s  <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (-1 == (mqd = mq_open(argv[1], O_RDONLY))) {
        printf("mq_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (-1 == mq_getattr(mqd, &attr)) {
        printf("mq_getattr error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("maxmsg = %ld, msgsize = %ld, curmsgs = %ld\n", attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
    if (-1 == mq_close(mqd)) {
        printf("mq_close error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

