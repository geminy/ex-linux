#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

mqd_t mqd;
void *buff;
struct mq_attr attr;
struct sigevent sigev;

static void sig_usr1(int);

int main(int argc, char **argv)
{
    if (2 != argc) {
        printf("usage: %s <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // open queue, get attributes, allocate read buffer
    if (-1 == (mqd = mq_open(argv[1], O_RDONLY))) {
        printf("mq_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (-1 == mq_getattr(mqd, &attr)) {
        printf("mq_getattr error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (NULL == (buff = malloc(attr.mq_msgsize))) {
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }
    // establish signal handler, enable notifation
    if (SIG_ERR == signal(SIGUSR1, sig_usr1)) {
        printf("signal error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    if (-1 == mq_notify(mqd, &sigev)) {
        printf("mq_notify error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (;;) {
        pause(); // signal handler does everything
    }
    exit(EXIT_SUCCESS);
}

// wrong with mq_notify/mq_receive/printf
// not async-signal-safe functions called from sig_urr1
static sig_usr1(int signo)
{
    ssize_t n;
    if (-1 == mq_notify(mqd, &sigev)) { // register first
        printf("mq_notify error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    n = mq_receive(mqd, buff, attr.mq_msgsize, NULL);
    printf("SIGUSR1 received, read %ld bytes\n", (long)n);
    return;
}
