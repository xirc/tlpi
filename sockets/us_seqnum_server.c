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
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "us_seqnum.h"

#define BACKLOG 5


static volatile sig_atomic_t is_running = 1;


static void
cleanup_handler(int sig __attribute__((unused)))
{
    is_running = 0;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int seq_num;        /* This is our "service" */
    struct sigaction sa;
    struct sockaddr_un svaddr, claddr;
    int sfd, cfd;
    ssize_t num_bytes;
    socklen_t len;
    struct request req;
    struct response resp;

    /* Set cleanup handler */
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = cleanup_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction - SIGINT");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction - SIGTERM");
        exit(EXIT_FAILURE);
    }

    /* Create server socket */
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Construct well-known address and bind server socket to it */
    if (remove(US_SERVER_PATH) == -1 && errno != ENOENT) {
        perror("remove - " US_SERVER_PATH);
        exit(EXIT_FAILURE);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, US_SERVER_PATH, sizeof(svaddr.sun_path) - 1);
    if (bind(sfd, (struct sockaddr *) &svaddr,
                sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    /* Read requests and send responses */
    seq_num = 0;
    while (is_running) {
        cfd = accept(sfd, (struct sockaddr *) &claddr, &len);
        if (cfd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        /* Receive a request */
        num_bytes = recv(cfd, &req, sizeof(struct request), 0);
        if (num_bytes == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        printf("Server received request from %s\n", claddr.sun_path);

        /* Send a response */
        resp.seq_num = seq_num;
        if (send(cfd, &resp, sizeof(struct response), 0) == -1)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }

        /* Close the socket */
        (void) close(cfd);

        seq_num += req.seq_len;
    }

    exit(EXIT_SUCCESS);
}
