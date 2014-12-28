/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef PMSG_SEQNUM_H
#define PMSG_SEQNUM_H


#include <sys/types.h>
#include <unistd.h>


#define SERVER_PMSG "/seqnum_server"
        /* Well-known name for server's PMSG */
#define CLIENT_PMSG_TEMPLATE "/seqnum_client.%ld"
        /* Template for building client PMSG name */
#define CLIENT_PMSG_NAME_LEN (sizeof(CLIENT_PMSG_TEMPLATE) + 20)
        /* Space required for client PMSG pathname
         * (+20 as a generous allowance for the PID */


struct request {        /* Request (client --> server) */
    pid_t pid;          /* PID of client */
    int seq_len;        /* Length of desired sequence */
};


struct response {       /* Response (server --> client) */
    int seq_num;        /* Start of sequence */
};


#endif
