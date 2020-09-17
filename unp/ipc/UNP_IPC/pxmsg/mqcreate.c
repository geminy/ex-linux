#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

// mq_maxmsg and mq_msgsize both init to 0
struct mq_attr attr;

int main(int argc, char **argv)
{
    int c, flags;
    mqd_t mqd;
    flags = O_RDWR | O_CREAT;
    while (-1 != (c = getopt(argc, argv, "em:z:"))) {
        switch (c) {
        case 'e':
            flags |= O_EXCL;
            break;
        case 'm':
            attr.mq_maxmsg = atol(optarg);
            break;
        case 'z':
            attr.mq_msgsize = atol(optarg);
            break;
        }
    }
    if (optind != argc - 1) {
        printf("usage: %s [-e] [-m maxmsg -z msgsize] <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if ((attr.mq_maxmsg != 0 && attr.mq_msgsize == 0) || (attr.mq_maxmsg == 0 && attr.mq_msgsize != 0)) {
        printf("must specify both -m maxmsg and -z msgsize\n");
        exit(EXIT_FAILURE);
    }
    mqd = mq_open(argv[optind], flags, FILE_MODE, (attr.mq_maxmsg != 0) ? &attr : NULL);
    if (-1 == mqd) {
        printf("mq_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (-1 == mq_close(mqd)) {
        printf("mq_close error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
//    mq_unlink(argv[optind]);
    exit(EXIT_SUCCESS);
}
