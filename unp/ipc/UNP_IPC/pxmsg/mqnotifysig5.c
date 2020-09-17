#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>

int pipefd[2];
static void sig_usr1(int);

// using pipe and select
int main(int argc, char **argv)
{
    int nfds;
    char c;
    fd_set rset;
    mqd_t mqd;
    void *buff;
    ssize_t n;
    struct mq_attr attr;
    struct sigevent sigev;
    if (2 != argc) {
        printf("usage: %s <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // open queue, get attributes, allocate read buffer
    if (-1 == (mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK))) {
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
    if (0 > pipe(pipefd)) {
        printf("pipe error: %s\n", strerror(errno));
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
        FD_SET(pepefd[0], &rset);
        nfds = select(pipefd[0] + 1, &ret, NULL, NULL, NULL); // ?
        if (-1 == nfds) {
            printf("select error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(pipefd[0], &rset)) {
            read(pipefd[0], &c, 1);
            if (-1 == mq_notify(mqd, &sigev)) { // register first
                printf("mq_notify error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            while (0 <= (n = mq_receive(mqd, buff, attr.mq_msgsize, NULL))) {
                printf("SIGUSR1 received, read %ld bytes\n", (long)n);
            }
            if (EAGAIN == errno) {
                printf("mq_receive error"); // no msg for reading
            }
        }
    }
    exit(EXIT_SUCCESS);
}

static sig_usr1(int signo)
{
    write(pipefd[1], "", 1); // one byte of 0
    return;
}
