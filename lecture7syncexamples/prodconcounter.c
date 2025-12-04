//
// Created by osc on 5/11/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 12

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int counter = 0;

pthread_mutex_t mutex;
pthread_cond_t full;
pthread_cond_t empty;

void* producer(void* arg) {
    int item = 1;

    while (1) {
        pthread_mutex_lock(&mutex);

        while (counter == BUFFER_SIZE) {
            pthread_cond_wait(&empty, &mutex);
        }

        buffer[in] = item;
        printf("Produced: %d\n", item);
        sleep(1);
        item++;
        in = (in + 1) % BUFFER_SIZE;
        counter++;

        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    pthread_exit(NULL);
}

void* consumer(void* arg) {
    pthread_t tid = pthread_self();
    while (1) {
        pthread_mutex_lock(&mutex);

        while (counter == 0) {
            pthread_cond_wait(&full, &mutex);
        }

        int item = buffer[out];

        out = (out + 1) % BUFFER_SIZE;
        counter--;
        sleep(1);
        printf("Consumed: %d by %ld\n", item, tid);

        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);

        sleep(20);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t producerThread, consumer1Thread, consumer2Thread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);

    pthread_create(&producerThread, NULL, producer, NULL);
    pthread_create(&consumer1Thread, NULL, consumer, NULL);
    pthread_create(&consumer2Thread, NULL, consumer, NULL);

    pthread_join(producerThread, NULL);
    pthread_join(consumer1Thread, NULL);
    pthread_join(consumer2Thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    return 0;
}
