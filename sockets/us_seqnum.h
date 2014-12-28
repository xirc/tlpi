/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef US_SEQNUM_H
#define US_SEQNUM_H


#include <sys/types.h>
#include <unistd.h>


#define US_SERVER_PATH "/tmp/seqnum_server"
        /* Well-known name for server's UNIX domain socket */
#define US_CLIENT_PATH_TEMPLATE "/tmp/seqnum_client.%ld"
        /* Template for building client's UNIX domain socket */

struct request {        /* Request (client --> server) */
    int seq_len;        /* Length of desired sequence */
};

struct response {       /* Response (server --> client) */
    int seq_num;        /* Start of sequence */
};


#endif
