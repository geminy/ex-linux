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

mqd_t mqd;
struct mq_attr attr;
struct sigevent sigev;

static void notify_thread(union sigval); // our thread function

// using pthread
int main(int argc, char **argv)
{
    if (2 != argc) {
        printf("usage: %s <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (-1 == (mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK))) {
        printf("mq_open error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (-1 == mq_getattr(mqd, &attr)) {
        printf("mq_getattr error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = NULL;
    sigev.sigev_notify_function = notify_thread;
    sigev.sigev_notify_attributes = NULL;
    if (-1 == mq_notify(mqd, &sigev)) {
        printf("mq_notify error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (;;) {
        pause(); // each new thread does everything
    }
    exit(EXIT_SUCCESS);
}

static void notify_thread(union sigval arg)
{
    ssize_t n;
    void *buff;
    printf("notify_thread started\n");
    buff = malloc(attr.mq_msgsize);
    if (-1 == mq_notify(mqd, &sigev)) { // register
        printf("mq_notify error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    while (0 <= (n = mq_receive(mqd, buff, attr.mq_msgsize, NULL))) {
        printf("read %ld bytes\n", (long)n);
    }
    if (EAGAIN == errno) {
        printf("mq_receive error");
    }
    free(buff);
    pthread_exit(NULL);
}
