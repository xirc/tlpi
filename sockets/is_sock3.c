/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "inet_sockets.h"


#define BUF_SIZE 1024


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int retc;
    int sfda, sfdb, sfdc;
    struct sockaddr_in addr;
    char buf[BUF_SIZE];

    sfda = inet_bind("10001", SOCK_DGRAM, NULL);
    if (sfda == -1) {
        perror("inet_bind");
        exit(EXIT_FAILURE);
    }

    sfdb = inet_bind("10002", SOCK_DGRAM, NULL);
    if (sfdb == -1) {
        perror("inet_bind");
        exit(EXIT_FAILURE);
    }

    sfdc = inet_bind("10003", SOCK_DGRAM, NULL);
    if (sfdc == -1) {
        perror("inet_bind");
        exit(EXIT_FAILURE);
    }

    printf("Bind (10001, 10002, 10003)\n");

    /* (fail) b --> a */
    retc = send(sfdb, "a", 1, 0);
    if (retc == 0 || (retc == -1 && errno != EDESTADDRREQ)) {
        printf("unexpected success or error (errno=%d)\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("TEST PASS\n");

    /* (connect) b --> a */
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10001);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    if (connect(sfdb, (struct sockaddr *) &addr,
                sizeof(struct sockaddr_in6)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    /* (send) b --> a */
    if (send(sfdb, "a", 1, 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }
    /* (receive) a <-- ? */
    if (recv(sfda, buf, BUF_SIZE, MSG_DONTWAIT) == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    if (memcmp("a", buf, 1) != 0) {
        fprintf(stderr, "WRONG RECV data\n");
        exit(EXIT_FAILURE);
    }
    printf("TEST PASS\n");

    /* c --> b */
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10002);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    if (connect(sfdc, (struct sockaddr *) &addr,
                sizeof(struct sockaddr_in)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    if (sendto(sfdc, "b", 1, 0, (struct sockaddr *) &addr,
                sizeof(struct sockaddr_in6)) == -1)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    printf("TEST PASS\n");

    retc = recv(sfdb, buf, BUF_SIZE, MSG_DONTWAIT);
    if (retc == 0 ||
        (retc == -1 && errno != EAGAIN))
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    printf("TEST PASS\n");

    printf("(ALL) TEST PASS\n");
    exit(EXIT_SUCCESS);
}
