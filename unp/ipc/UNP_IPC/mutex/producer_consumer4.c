#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAXITEMS (100000)
#define MAXTHREADS (10)
#define min(a, b) ((a) > (b) ? (b) : (a))

int g_itemnum;
int g_buff[MAXITEMS];

struct
{
    pthread_mutex_t mutex;
    int nindex; // next index in g_buff
    int nvalue; // next value in g_buff
} shared = { PTHREAD_MUTEX_INITIALIZER };

struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int ready; // if ready for consumer
} ready = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER };

void* producer(void*);
void* consumer(void*);

int main(int argc, char **argv)
{
    int i;
    int threadnum, threadcount[MAXTHREADS];
    pthread_t tid_producer[MAXTHREADS], tid_consumer;

    if (3 != argc) {
        printf("usage: %s <item_num> <thread_num>\n", argv[0]);
    }
    g_itemnum = min(atoi(argv[1]), MAXITEMS);
    threadnum = min(atoi(argv[2]), MAXTHREADS);
    printf("item = %d, thread = %d\n", g_itemnum, threadnum);
    pthread_setconcurrency(threadnum + 1);

    // start all the producer threads
    for (i = 0; i < threadnum; ++i) {
        threadcount[i] = 0;
        if (0 != pthread_create(&tid_producer[i], NULL, producer, (void*)&threadcount[i])) {
            printf("pthread_create error producer %d\n", i);
            exit(EXIT_FAILURE);
        }
        printf("producer: thread[%lu] created, threadcount[%d] = %d\n", tid_producer[i], i, threadcount[i]);
    }
    // start consumer thread
    if (0 != pthread_create(&tid_consumer, NULL, consumer, NULL)) {
        printf("pthread_create error consumer\n");
    }
    printf("consumer: thread[%lu] created\n", tid_consumer);

    // wait for all the producer threads
    for (i = 0; i < threadnum; ++i) {
        if (0 != pthread_join(tid_producer[i], NULL)) {
            printf("pthread_join error producer %d\n", i);
            exit(EXIT_FAILURE);
        }
        printf("producer: thread[%lu] done, threadcount[%d] = %d\n", tid_producer[i], i, threadcount[i]);
    }
    // wait for the consumer thread
    if (0 != pthread_join(tid_consumer, NULL)) {
        printf("pthread_join error consumer\n");
    }
    printf("consumer: thread[%lu] done\n", tid_consumer);
    exit(EXIT_SUCCESS);
}

void* producer(void *arg)
{
    for (;;) {
        pthread_mutex_lock(&shared.mutex);
        if (shared.nindex >= g_itemnum) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }
        g_buff[shared.nindex] = shared.nvalue;
        shared.nindex++;
        shared.nvalue++;
        pthread_mutex_unlock(&shared.mutex);

        // cond
        pthread_mutex_lock(&ready.mutex);
        if (0 == ready.ready) {
            pthread_cond_signal(&ready.cond);
        }
        ready.ready++;
        pthread_mutex_unlock(&ready.mutex);

        *((int*)arg) += 1;
    }
    return NULL;
}

void* consumer(void *arg)
{
    int i;
    for (i = 0; i < g_itemnum; ++i) {
        // cond
        pthread_mutex_lock(&ready.mutex);
        while (0 == ready.ready) {
            pthread_cond_wait(&ready.cond, &ready.mutex);
        }
        ready.ready--;
        pthread_mutex_unlock(&ready.mutex);

        if (g_buff[i] != i) {
            printf("error: buff[%d] = %d\n", i, g_buff[i]);
        }
    }
    return NULL;
}
