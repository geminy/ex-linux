#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define NBUFF (10) // length for shared buffer

int nitems; // products number by producer
struct
{
    int buff[NBUFF]; // shared buffer
    sem_t *mutex; // binary semaphore, initial value is 1
    sem_t *nempty; // countable semaphore, initial value is NBUFF
    sem_t *nstored; // countable semaphore, initial value is 0
} shared;

void* producer(void*);
void* consumer(void*);

int main(int argc, char **argv)
{
    pthread_t tid_producer, tid_consumer;
    int count_prod, count_cons;
    if (2 != argc) {
        printf("usage: %s <items>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    nitems = atoi(argv[1]);

    // creates three semaphores
    // "/mutex"
    if ((shared.mutex = sem_open("/mutex", O_CREAT | O_EXCL, FILE_MODE, 1)) == SEM_FAILED) {
        printf("sem_open /mutex error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else {
        printf("semaphore /mutex created\n");
    }
    // "/nempty"
    if ((shared.nempty = sem_open("/nempty", O_CREAT | O_EXCL, FILE_MODE, NBUFF)) == SEM_FAILED) {
        printf("sem_open /nempty error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else {
        printf("semaphore /nempty created\n");
    }
    // "/nstored"
    if ((shared.nstored = sem_open("/nstored", O_CREAT | O_EXCL, FILE_MODE, 0)) == SEM_FAILED) {
        printf("sem_open /nstored error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else {
        printf("semaphore /nstored created\n");
    }

    // creates and waits for producer and consumer threads
    if (0 != pthread_setconcurrency(2)) {
        printf("pthread_setconcurrency error\n");
    }
    // producer create
    count_prod = 0;
    if (0 != pthread_create(&tid_producer, NULL, producer, (void*)&count_prod)) {
        printf("pthread_create error: producer\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("producer [%lu] created, count = %d\n", tid_producer, count_prod);
    }
    // consumer create
    count_cons = 0;
    if (0 != pthread_create(&tid_consumer, NULL, consumer, (void*)&count_cons)) {
        printf("pthread_create error: consumer\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("consumer [%lu] created, count = %d\n", tid_consumer, count_cons);
    }
    // producer join
    if (0 != pthread_join(tid_producer, NULL)) {
        printf("pthread_join error: producer\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("producer [%lu] done, count = %d\n", tid_producer, count_prod);
    }
    // consumer join
    if (0 != pthread_join(tid_consumer, NULL)) {
        printf("pthread_join error: producer\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("consumer [%lu] done, count = %d\n", tid_consumer, count_cons);
    }

    // removes the three created semaphores
    // "/mutex"
    if (0 != sem_unlink("/mutex")) {
        printf("sem_unlink /mutex error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else {
        printf("semaphore /mutex removed\n");
    }
    // "/nempty"
    if (0 != sem_unlink("/nempty")) {
        printf("sem_unlink /nempty error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else {
        printf("semaphore /nempty removed\n");
    }
    // "/nstored"
    if (0 != sem_unlink("/nstored")) {
        printf("sem_unlink /nstored error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else {
        printf("semaphore /nstored removed\n");
    }

    exit(EXIT_SUCCESS);
}

void* producer(void *arg)
{
    int i;
    for (i = 0; i < nitems; ++i) {
        if (sem_wait(shared.nempty) == -1) { // wait for at least 1 empty slot
            printf("sem_wait nempty error: %s\n", strerror(errno));
        }
        if (sem_wait(shared.mutex) == -1) { // mutex lock
            printf("sem_wait mutex error: %s\n", strerror(errno));
        }
        shared.buff[i % NBUFF] = i; // store i into circular buffer
        *((int*)arg) += 1;
        if (sem_post(shared.mutex) == -1) { // mutex unlock
            printf("sem_post mutex error: %s\n", strerror(errno));
        }
        if (sem_post(shared.nstored) == -1) { // 1 more stored item
            printf("sem_post nstored error: %s\n", strerror(errno));
        }
    }
    return NULL;
}

void* consumer(void *arg)
{
    int i;
    for (i = 0; i < nitems; ++i) {
        if (sem_wait(shared.nstored) == -1) { // wait for at least 1 stored item
            printf("sem_wait nstored error: %s\n", strerror(errno));
        }
        if (sem_wait(shared.mutex) == -1) { // mutex lock
            printf("sem_wait mutex error: %s\n", strerror(errno));
        }
        if (shared.buff[i % NBUFF] != i) { // buffer check
            printf("error: buff[%d] = %d\n", i, shared.buff[i % NBUFF]);
        }
        *((int*)arg) += 1;
        if (sem_post(shared.mutex) == -1) { // mutex unlock
            printf("sem_post mutex error: %s\n", strerror(errno));
        }
        if (sem_post(shared.nempty) == -1) { // 1 more empty slot
            printf("sem_post nempty error: %s\n", strerror(errno));
        }
    }
    return NULL;
}
