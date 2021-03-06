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


/* is_seqnum_sv.c

   A simple Internet stream socket server. Our service is to provide
   unique sequence numbers to clients.

   Usage:  is_seqnum_sv [init-seq-num]
                        (default = 0)

   See also is_seqnum_cl.c.
*/


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
#include "read_line.h"

#define BACKLOG 50


int
main(int argc, char *argv[])
{
    uint32_t seqnum;
    char reqlen_str[INT_LEN];       /* Length of requested sequence */
    char seqnum_str[INT_LEN];       /* Start of granted sequence */
    struct sockaddr_storage claddr;
    int s, lfd, cfd, optval, reqlen;
    socklen_t addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
    char addr_str[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [init-seq-num]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    seqnum = (argc > 1) ? strtol(argv[1], NULL, 0) : 0;
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    /* Call getaddrinfo() to obtain a list of addresses that
     * we can try binding to
     */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;    /* Allows IPv4 or IPv6 */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
        /* Wildcard IP address; service name is numeric */

    s = getaddrinfo(NULL, PORT_NUM, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* Walk through returned list until we find an address structure
     * that can be used to successfully created and bind a socket */
    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1) {
            /* On error, try next address */
            continue;
        }

        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR,
                    &optval, sizeof(optval)) == -1)
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            /* Success */
            break;
        }
        /* bind() failed: close this socket and try next address */
        (void) close(lfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not bind socket to any address\n");
        exit(EXIT_FAILURE);
    }

    if (listen(lfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    /* Handle clients iteratively */
    while (1) {
        /* Accept a client connection, obtaining client's address */
        addrlen = sizeof(struct sockaddr_storage);
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
        if (read_line(cfd, reqlen_str, INT_LEN) <= 0) {
            /* Failed read; skip request */
            close(cfd);
            continue;
        }

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
