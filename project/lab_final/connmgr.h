//
// Created by martin on 12/13/25.
//

#ifndef STUDENTSOURCE2025_CONNMGR_H
#define STUDENTSOURCE2025_CONNMGR_H
#include "lib/tcpsock.h"
#include "sbuffer.h"

typedef struct conn_block { // Needed for synchronization between main thread and connection manager's server thread
    sbuffer_t * buffer;
    int max_conn;
    int port_nr;
} conn_block_t;

typedef struct client_block { // Needed for synchronization between  connmgr's server thread and connmgr's client threads
    tcpsock_t * client;
    sbuffer_t * buffer;
} client_block_t;

int connection (sbuffer_t * sbuff, int MAX_CONN, int PORT);

void * worker(void *client);

#endif //STUDENTSOURCE2025_CONNMGR_H
