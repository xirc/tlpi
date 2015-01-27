/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
    /* for netdb.h/NI_MAX{SERV|HOST} */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int sockfd;
    int retc;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    retc = listen(sockfd, /* BACKLOG */ 5);
    if (retc == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    retc = getsockname(sockfd, (struct sockaddr *) &addr, &addrlen);
    if (retc == -1) {
        perror("getsockname");
        exit(EXIT_FAILURE);
    }

    retc = getnameinfo((struct sockaddr *) &addr, addrlen,
            host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    if (retc == -1) {
        perror("getnameinfo");
        exit(EXIT_FAILURE);
    }
    printf("listen socket: %s %s\n", host, serv);

    exit(EXIT_SUCCESS);
}
