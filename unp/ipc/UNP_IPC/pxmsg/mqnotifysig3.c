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

// set nonzero by signal handler
volatile sig_atomic_t mqflag;
static void sig_usr1(int);

// read msg with nonblock while mq_notify
int main(int argc, char **argv)
{
    mqd_t mqd;
    void *buff;
    ssize_t n;
    sigset_t zeromask, newmask, oldmask;
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
    sigemptyset(&zeromask); // no signals blocked
    sigemptyset(&newmask);
    sigemptyset(&oldmask);
    sigaddset(&newmask, SIGUSR1);
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
        sigprocmask(SIG_BLOCK, &newmask, &oldmask); // block SIGUSR1
        while (mqflag == 0) {
            sigsuspend(&zeromask);
        }
        mqflag = 0; // reset flag
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
        sigprocmask(SIG_UNBLOCK, &newmask, NULL); // unblock SIGUSR1
    }
    exit(EXIT_SUCCESS);
}

// avoid function called from signal handler
// just set a global mark checked by some thread
static sig_usr1(int signo)
{
    mqflag = 1;
    return;
}
