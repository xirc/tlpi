/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef SVMSG_SEQNUM
#define SVMSG_SEQNUM


#include <sys/types.h>
#include <stddef.h>


#define MSQ_KEY   0x20141019
#define SERVER_ID 1


struct request {        /* Request (client --> server) */
    long mtype;         /* Reserved */
    pid_t pid;            /* PID of source */
    unsigned long seq_len;        /* Length of desired sequence */
};
#define REQ_MSG_SIZE (offsetof(struct request, seq_len) - \
                      offsetof(struct request, pid) + sizeof(int) /* sizeof seq_len */)


struct response {       /* Response (server --> client) */
    long mtype;         /* Reserved */
    pid_t pid;            /* PID of destination */
    unsigned long seq_num;        /* Start of sequence */
};
#define RESP_MSG_SIZE (offsetof(struct response, seq_num) - \
                       offsetof(struct response, pid) + sizeof(int) /* sizeof seq_num */)


#endif
