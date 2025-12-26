/**
 * \author {AUTHOR}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include <pthread.h>
#include <sys/socket.h>

/**
 * Implements a sequential test server (only one connection at the same time)
 */
//
// Created by martin on 12/13/25.
//



void * runner(void * client) { // Worker thread retrieves data from the client

	tcpsock_t * c = (tcpsock_t *)client; //Needed because API only accepts variables of type tcpsock_t
	sensor_data_t data;
	int bytes, result;
	do {
		// read sensor ID
		bytes = sizeof(data.id);
		result = tcp_receive(c, (void *) &data.id, &bytes); // Thread blocks here waiting for bytes
		// read temperature
		bytes = sizeof(data.value);
		result = tcp_receive(c, (void *) &data.value, &bytes);
		// read timestamp
		bytes = sizeof(data.ts);
		result = tcp_receive(c, (void *) &data.ts, &bytes); // If  thread is blocked here when connection is closed by client, tcp sends a signal to kernel, which will immediately return the function and allow the thread to continue
		if ((result == TCP_NO_ERROR) && bytes) {
			printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
				   (long int) data.ts);
		}
	} while (result == TCP_NO_ERROR); //  Connection between server and client is still alive and bytes being received.
	if (result == TCP_CONNECTION_CLOSED)
		printf("Peer has closed connection\n");
	else
		printf("Error occured on connection to peer\n");
	tcp_close(&c); // Server closes its end of the connection
	return NULL; // Exit point an return of parallel thread
}

int main(int argc, char *argv[]) {

	int MAX_CONN = atoi(argv[2]);
	int PORT = atoi(argv[1]);
	pthread_t tid[MAX_CONN];

    if(argc < 3) {
    	printf("Please provide the right arguments: first the port, then the max nb of clients");
    	return -1;
    }

	tcpsock_t *server, *client;
	int conn_counter = 0;

    printf("Test server is started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE); // Open server's TCP socket with specified port
    do {
	    if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE); // Main thread blocks at this line until new connection arrives. Then, client variable is overrwritten with new peer.
    	printf("Incoming client connection\n");
    	pthread_create(&tid[conn_counter], NULL, runner, client); // Thread tid[conn_counter] goes onto runner and executes there.
    	conn_counter++; // Main thread carries on execution, waiting for new connections.
    } while (conn_counter < MAX_CONN);
	for (int i = 0; i<MAX_CONN; i++) {
		pthread_join(tid[i], NULL); // Main thread gets trapped here and waits for thread tid[0]. When that one finishes "runner" execution, pthread_join returns. Then, main thread waits for tid[1], etc...
	}
    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Test server is shutting down\n");
    return 0;
}




