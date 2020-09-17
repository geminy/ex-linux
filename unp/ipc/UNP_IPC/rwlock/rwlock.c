#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define MAXSIZE (10000)

struct
{
    pthread_rwlock_t rwlock;
    int buff[MAXSIZE];
    int nindex; // next index of buff
    int nvalue; // next value of buff
} shared = { PTHREAD_RWLOCK_INITIALIZER };

void* thread_write(void *arg)
{
    while (1) {
        pthread_rwlock_wrlock(&shared.rwlock);
        if (shared.nindex >= MAXSIZE) {
            printf("write thread [%ld] full\n", pthread_self());
            pthread_rwlock_unlock(&shared.rwlock);
            return NULL;
        }
        shared.buff[shared.nindex] = shared.nvalue;
        shared.nindex++;
        shared.nvalue++;
        *((int*)arg) += 1;
        pthread_rwlock_unlock(&shared.rwlock);
    }
}

void* thread_read(void *arg)
{
    int i;
    time_t start = time(NULL);
    while (1) {
        pthread_rwlock_rdlock(&shared.rwlock);
        for (i = 0; i < shared.nindex; ++i) {
            if (shared.buff[i] != i) {
                printf("read thread [%ld] error: buff[%d]=%d\n", pthread_self(), i, shared.buff[i]);
            }
        }
        *((int*)arg) += 1;
        if (shared.nindex >= MAXSIZE) {
            printf("read thread [%ld] full\n", pthread_self());
            pthread_rwlock_unlock(&shared.rwlock);
            return NULL;
        }
        if (start + 5 < time(NULL)) {
            printf("read thread [%ld] timeout\n", pthread_self());
            pthread_rwlock_unlock(&shared.rwlock);
            return NULL;
        }
        pthread_rwlock_unlock(&shared.rwlock);
    }
}

int main(int argc, char *argv[])
{
    pthread_t wrthreads[2], rdthreads[2];
    int wrcount[2], rdcount[2];
    int s, i;

    // creates write threads
    for (i = 0; i < 2; ++i) {
        wrcount[i] = 0;
        if ((s = pthread_create(&wrthreads[i], NULL, thread_write, (void*)&wrcount[i])) != 0) {
            handle_error_en(s, "pthread_create write");
        }
        else {
            printf("write thread [%ld] created, count [%d]\n", wrthreads[i], wrcount[i]);
        }
    }

    // creates read threads
    for (i = 0; i < 2; ++i) {
        rdcount[i] = 0;
        if ((s = pthread_create(&rdthreads[i], NULL, thread_read, (void*)&rdcount[i])) != 0) {
            handle_error_en(s, "pthread_create read");
        }
        else {
            printf("read thread [%ld] created, count [%d]\n", rdthreads[i], rdcount[i]);
        }
    }

    // waiting for write threads
    for (i = 0; i < 2; ++i) {
        if ((s = pthread_join(wrthreads[i], NULL)) != 0) {
            handle_error_en(s, "pthread_join write");
        }
        else {
            printf("write thread [%ld] done, count [%d]\n", wrthreads[i], wrcount[i]);
        }
    }

    // waiting for read threads
    for (i = 0; i < 2; ++i) {
        if ((s = pthread_join(rdthreads[i], NULL)) != 0) {
            handle_error_en(s, "pthread_join read");
        }
        else {
            printf("read thread [%ld] done, count [%d]\n", rdthreads[i], rdcount[i]);
        }
    }

    printf("----------done----------\n");
    exit(EXIT_SUCCESS);
}
