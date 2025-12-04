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


pthread_mutex_t mutex;
pthread_cond_t full;
pthread_cond_t empty;

void* producer(void* arg) {
    int item = 1;

    while (1) {
        pthread_mutex_lock(&mutex);

        while (((in + 1) % BUFFER_SIZE) == out) {
            pthread_cond_wait(&empty, &mutex);
        }

        buffer[in] = item;
        printf("Produced: %d\n", item);
        sleep(1);
        item++;
        in = (in + 1) % BUFFER_SIZE;

        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    pthread_exit(NULL);
}

void* consumer(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (in == out) {
            pthread_cond_wait(&full, &mutex);
        }

        int item = buffer[out];

        out = (out + 1) % BUFFER_SIZE;
        sleep(1);
        printf("Consumed: %d\n", item);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);

        sleep(10);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t producerThread, consumerThread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);

    pthread_create(&producerThread, NULL, producer, NULL);
    pthread_create(&consumerThread, NULL, consumer, NULL);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    return 0;
}
