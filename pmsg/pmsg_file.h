/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef PMSG_FILE
#define PMSG_FILE


#include <sys/types.h>
#include <limits.h>


#define SERVER_QUEUE_PATH "/file_server"
#define CLIENT_PMSG_TEMPLATE "/file_client.%ld"
        /* Template for building client PMSG name */
#define CLIENT_PMSG_NAME_LEN (sizeof(CLIENT_PMSG_TEMPLATE) + 20)
        /* Space required for client PMSG pathname
         * (+20 as a generous allowance for the PID */


/* Requests (client --> server) */
struct request {
    int client_id;              /* ID of client's message queue */
    char pathname[PATH_MAX];    /* File to be returned */
};


/* Responses (server --> client) */
#define RESP_MSG_SIZE 4096
struct response {
    long mtype;                 /* One of RESP_MT_* values bellow */
    char data[RESP_MSG_SIZE];   /* File content / response message */
};


/* Types for response messages sent from server to client */
#define RESP_MT_FAILURE     1       /* File couldn't be opened */
#define RESP_MT_DATA        2       /* Message contains file data */
#define RESP_MT_END         3       /* File data complete */


#endif
