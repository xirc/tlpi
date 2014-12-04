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


#define _BSD_SOURCE
    /* To get definitions of NI_MAXHOST and
     * NI_MAXSERV from <netdb.h>
     */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>

#include "is_seqnum.h"
#include "readline2.h"
#include "inet_sockets.h"

#define BUFSIZE 4096
#define BACKLOG 50


int
main(int argc, char *argv[])
{
    uint32_t seqnum;
    char reqlen_str[INT_LEN];       /* Length of requested sequence */
    char seqnum_str[INT_LEN];       /* Start of granted sequence */
    struct sockaddr_storage claddr;
    int lfd, cfd, reqlen;
    socklen_t addrlen;
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
    char addr_str[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    struct rlbuf rlbuf;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [init-seq-num]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    seqnum = (argc > 1) ? strtol(argv[1], NULL, 0) : 0;
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    lfd = inet_listen(PORT_NUM, BACKLOG, &addrlen);
    if (lfd == -1) {
        perror("inet_listen");
        exit(EXIT_FAILURE);
    }

    /* Handle clients iteratively */
    while (1) {
        /* Accept a client connection, obtaining client's address */
        cfd = accept(lfd, (struct sockaddr *) &claddr, &addrlen);
        if (cfd == -1) {
            perror("accept");
            continue;
        }

        if (getnameinfo((struct sockaddr *) &claddr, addrlen,
                host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
        {
            snprintf(addr_str, ADDRSTRLEN, "(%s %s)", host, service);
        } else {
            snprintf(addr_str, ADDRSTRLEN, "(?UNKNOWN?)");
        }
        printf("Connection from %s\n", addr_str);

        /* Read client request, send sequence number back */
        if (readline_init(&rlbuf, cfd, BUFSIZE) == -1) {
            /* Failed to init; skip request */
            close(cfd);
            continue;
        }
        if (readline(&rlbuf, reqlen_str, INT_LEN) <= 0) {
            /* Failed read; skip request */
            readline_free(&rlbuf);
            continue;
        }
        readline_free(&rlbuf);

        reqlen = strtol(reqlen_str, NULL, 0);
        if (reqlen <= 0) {
            /* Watch for misbehaving clients.
             * Bad request; skip it */
            close(cfd);
            continue;
        }

        snprintf(seqnum_str, INT_LEN, "%d\n", seqnum);
        if (write(cfd, seqnum_str, strlen(seqnum_str)) !=
                (ssize_t) strlen(seqnum_str))
        {
            fprintf(stderr, "Error on write\n");
        }

        /* Update sequence number */
        seqnum += reqlen;
        if (close(cfd) == -1) {
            perror("close");
        }
    }


    exit(EXIT_SUCCESS);
}
