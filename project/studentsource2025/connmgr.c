//
// Created by martin on 12/13/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include <pthread.h>
#include <sys/socket.h>
#include "connmgr.h"

#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#include "sensor_db.h"

#ifndef TIMEOUT
#define TIMEOUT 4
#endif
extern int fd[2]; // Defined in storage manager. Accessed here for logs
tcpsock_t *server= NULL; // Defined here. Accessed by main.c to close the connection

void * worker(void * client_block_prmtr) { // Worker thread retrieves data from the client
	client_block_t *client_block_wrkr = (client_block_t *) client_block_prmtr;

	tcpsock_t * c = client_block_wrkr->client;
	sbuffer_t * sbuff = client_block_wrkr->buffer;

	sensor_data_t sensor_data;
	int bytes, result;
	int connection_established = 0;
	char log_msg[128];
	log_msg[0] = '\0';
	int sd;
	tcp_get_sd(c, &sd);

	do {
		fd_set readfds;
		struct timeval tv;

		FD_ZERO(&readfds);
		FD_SET(sd, &readfds);

		tv.tv_sec  = TIMEOUT;
		tv.tv_usec = 0;

		int ready = select(sd + 1, &readfds, NULL, NULL, &tv); // Waits up to tv seconds for socket activity. It also overwrites tv at every iteration, so you must redefine it at the top of each loop iteration.

		if (ready == 0) {
			// TIMEOUT expired. Close connection
			result = TCP_TIMEOUT;
			break;
		}
		if (ready < 0) {
			// select error
			perror("select");
			result = TCP_CONNECTION_CLOSED; // or TCP_ERROR
			break;
		}

		// Receive bytes.
		bytes = sizeof(sensor_data.id);
		result = tcp_receive(c, (void *) &sensor_data.id, &bytes); // Reading sensor ID. Thread blocks here waiting for bytes

		bytes = sizeof(sensor_data.value);
		result = tcp_receive(c, (void *) &sensor_data.value, &bytes); // Read temperature

		bytes = sizeof(sensor_data.ts);
		result = tcp_receive(c, (void *) &sensor_data.ts, &bytes); // Read timestamp. If  thread is blocked here when connection is closed by client, tcp sends a signal to kernel, which will immediately return the function and allow the thread to continue

		// LOG CONNECTION
		if (connection_established == 0) {
			snprintf(log_msg, sizeof(log_msg),
						"Sensor node %d has opened a new connection",
						sensor_data.id);
			write(fd[1],log_msg,strlen(log_msg)+1);
			connection_established = 1;
		}

		if (result == TCP_NO_ERROR) {
			if (bytes) {
				// printf("sensor id = %" PRIu16 ", temperature = %g, timestamp = %ld\n", sensor_data.id, sensor_data.value, (long int) sensor_data.ts);
				sbuffer_insert(sbuff,&sensor_data); // Insert sensor data in buffer
				// printf("***** ADDED: buffer size = %d ******\n", sbuffer_size(sbuff));
			}
		}
	} while (result == TCP_NO_ERROR); //  Connection between server and client is still alive and bytes being received.
	if (result == TCP_CONNECTION_CLOSED) {
		snprintf(log_msg, sizeof(log_msg),
				"Sensor node %d has closed the connection",
				sensor_data.id);
	}
	else if (result == TCP_TIMEOUT) {
		snprintf(log_msg, sizeof(log_msg),
			"TIMEOUT: Server has closed connection with Sensor %d due to inactivity",
			sensor_data.id);
	}
	else

	tcp_close(&c); // Server closes its end of the connection

	// LOG CONNECTION SHUTDOWN
	write(fd[1],log_msg,strlen(log_msg)+1);
	return NULL; // Exit point an return of parallel thread
}

int connection(sbuffer_t * sbuff, int MAX_CONN, int PORT) {
	tcpsock_t *client;
	int conncntr = 0;
	pthread_t tid[MAX_CONN];

	client_block_t * client_ptr_array[MAX_CONN];
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE); // Open server's TCP socket with specified port
    do {
	    if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) break; // Main thread blocks at this line until new connection arrives. Then, client variable is overrwritten with new peer.

    	client_ptr_array[conncntr] = malloc(sizeof *client_ptr_array[conncntr]);
    	if (!client_ptr_array[conncntr]) exit(EXIT_FAILURE);

    	client_ptr_array[conncntr]->buffer = sbuff;
    	client_ptr_array[conncntr]->client = client;

    	pthread_create(&tid[conncntr], NULL, worker, client_ptr_array[conncntr]);
    	conncntr++;

    } while (conncntr < MAX_CONN);
	for (int i = 0; i<conncntr; i++) {
		pthread_join(tid[i], NULL); // Main thread gets trapped here and waits for thread tid[0]. When that one finishes "runner" execution, pthread_join returns. Then, main thread waits for tid[1], etc...
		free(client_ptr_array[i]);
	}

    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);
	sbuff_connmgr_termination(sbuff);
	return 0;
}