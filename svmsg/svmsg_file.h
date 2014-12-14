/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* svmsg_file.h

   Header file for svmsg_file_server.c and svmsg_file_client.c.
*/


#ifndef SVMSG_FILE
#define SVMSG_FILE


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>


#define SERVER_KEY  0x1aaaaaa1 /* Key for server's message queue */


/* Requests (client --> server) */
struct request_msg {
    long mtype;                 /* unused */
    int client_id;              /* ID of client's message queue */
    char pathname[PATH_MAX];    /* File to be returned */
};


/*
 * REQ_MSG_SIZE computes size of 'mtext' part of 'request_msg' structure.
 * We use offsetof() to handle the possibility that there are padding
 * bytes between the 'client_id' and 'pathname' fields.
 */
#define REQ_MSG_SIZE  (offsetof(struct request_msg, pathname) - \
                       offsetof(struct request_msg, client_id) + PATH_MAX)

#define RESP_MSG_SIZE 8192


/* Responses (server --> client) */
struct response_msg {
    long mtype;                 /* One of RESP_MT_* values bellow */
    char data[RESP_MSG_SIZE];   /* File content / response message */
};


/* Types for response messages sent from server to client */
#define RESP_MT_FAILURE     1       /* File couldn't be opened */
#define RESP_MT_DATA        2       /* Message contains file data */
#define RESP_MT_END         3       /* File data complete */


#endif
