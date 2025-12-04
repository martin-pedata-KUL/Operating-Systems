//
// Created by osc on 5/11/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 12

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t mutex;
sem_t full;
sem_t empty;

void* producer(void* arg) {
    int item = 1;

    while (1) {

        sem_wait(&empty);
        sem_wait(&mutex);

        buffer[in] = item;
        printf("Produced: %d\n", item);
        sleep(1);
        item++;
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&full);

        sleep(2);
    }

    pthread_exit(NULL);
}

void* consumer(void* arg) {
    pthread_t tid = pthread_self();
    while (1) {

        sem_wait(&full);
        sem_wait(&mutex);

        int item = buffer[out];
        buffer[out] = -1;
        printf("Consumed: %d by %ld\n", item, tid);
        sleep(1);

        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&empty);

      sleep(30);

    }

    pthread_exit(NULL);
}

int main() {
    pthread_t producerThread, consumer1Thread,consumer2Thread,consumer3Thread;

    sem_init(&mutex,1,1);
    sem_init(&full, 1,0);
    sem_init(&empty, 1, BUFFER_SIZE);

    pthread_create(&producerThread, NULL, producer, NULL);
    pthread_create(&consumer1Thread, NULL, consumer, NULL);
    pthread_create(&consumer2Thread, NULL, consumer, NULL);
    pthread_create(&consumer3Thread, NULL, consumer, NULL);

    pthread_join(producerThread, NULL);
    pthread_join(consumer1Thread, NULL);
    pthread_join(consumer2Thread, NULL);
    pthread_join(consumer3Thread, NULL);

    sem_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);

    return 0;
}
