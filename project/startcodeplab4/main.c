//
// Created by martin on 12/4/25.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include "sbuffer.h"
#include <unistd.h>
#include <string.h>

int in = 0;
int out = 0;

extern pthread_mutex_t wout;
pthread_mutex_t file_mutex;
pthread_cond_t not_empty;

void * producer (void * sbuffer) {
    sbuffer_t * sbuff = (sbuffer_t *) sbuffer;
    if (sbuff == NULL) return NULL;
    FILE * fp_sensor_data = fopen("sensor_data", "rb");
    sensor_data_t data;

    while (1) {
        pthread_mutex_lock(&wout);

        sensor_id_t sensor_id;
        sensor_ts_t sensor_ts;
        sensor_value_t sensor_temp;

        // Read id (2 bytes)
        if (fread(&sensor_id, sizeof(uint16_t), 1, fp_sensor_data) != 1) // In this line of code, two functions are executed. Firstly, the allocation of the next bytes to the local variable sensor_id_rb. Secondly, the check that there are indeed successive bytes to read from.
            {
            pthread_mutex_unlock(&wout);
            break; // reached EOF or read error
            }

        // Read temperature (8 bytes)
        if (fread(&sensor_temp, sizeof(double), 1, fp_sensor_data) != 1) {
            pthread_mutex_unlock(&wout);
            break; // read error (no corresponding binary value found in file)
        }

        // Read timestamp (sizeof(time_t) bytes. 8 in my case)
        if (fread(&sensor_ts, sizeof(time_t), 1, fp_sensor_data) != 1){
            pthread_mutex_unlock(&wout);
            break; // idem
            }

        //update the data to the current reading
        data.id = sensor_id;
        data.ts = sensor_ts;
        data.value = sensor_temp;

        //Update dynamic pointer list, signal waiting threads, and release the lock
        sbuffer_insert(sbuff,&data);
        pthread_cond_signal(&not_empty);     // notify consumers

        pthread_mutex_unlock(&wout);

        //usleep(10000);
    }

    fclose(fp_sensor_data);

    //end of stream marker
    data.id = 0;
    data.ts = 0;
    data.value = 0;
    pthread_mutex_lock(&wout);
    sbuffer_insert(sbuff,&data); // Insert ending signal to the buffer
    pthread_cond_signal(&not_empty);     // notify consumers
    pthread_mutex_unlock(&wout);

    pthread_exit(NULL);
}

void * consumer (void * sbuffer) {
    sbuffer_t * sbuff = (sbuffer_t *) sbuffer;
    if (sbuff == NULL) return NULL;
    sensor_data_t data;
    FILE * f_log = fopen("sensor_data_out.csv", "a");
    while (1) {
        while (sbuffer_remove(sbuff, &data) == SBUFFER_NO_DATA) { // // Waits till sbuffer has at least one element. Then, 'data' variable is overwritten with the sensor recording.
            pthread_cond_wait(&not_empty, &wout);
        }

        if (data.id == 0) { // End of stream
            sbuffer_insert(sbuff, &data);
            pthread_mutex_unlock(&wout);
            break;
        }

        //Write (atomically) into csv
        pthread_mutex_lock(&file_mutex);
        fprintf(f_log, "%u,", data.id); // fprintf() -> system call to write into OPENED file.
        fprintf(f_log, "%.10f,", data.value);
        fprintf(f_log, "%ld\n", data.ts); // CHECK LONG
        pthread_mutex_unlock(&file_mutex);

        int flush = fflush(f_log);

        //usleep(25000);
    }

    fclose(f_log);
    pthread_exit(NULL);
}

int main() {

    pthread_t writerThread, reader1Thread, reader2Thread;
    sbuffer_t * sbuffer = NULL;

    sbuffer_init(&sbuffer);

    pthread_mutex_init(&wout, NULL);
    pthread_mutex_init(&file_mutex, NULL);
    pthread_cond_init(&not_empty, NULL);

    pthread_create(&writerThread, NULL, producer, sbuffer);
    pthread_create(&reader1Thread, NULL, consumer, sbuffer);
    pthread_create(&reader2Thread, NULL, consumer, sbuffer);

    pthread_join(writerThread, NULL);
    pthread_join(reader1Thread, NULL);
    pthread_join(reader2Thread, NULL);

    sbuffer_free(&sbuffer);
    return 0;
}