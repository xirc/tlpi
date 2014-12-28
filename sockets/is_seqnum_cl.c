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


/* is_seqnum_cl.c

   A simple Internet stream socket client. This client requests a sequence
   number from the server.

   See also is_seqnum_sv.c.
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


int
main(int argc, char *argv[])
{
    char *reqlen_str;           /* Requested length of sequence */
    char seqnum_str[INT_LEN];    /* Start of granted sequence */
    int cfd, s;
    ssize_t num_read;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s server-host [sequence-len]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Call getaddrinfo() to obtain a list of addresses that
     * we can try connecting to */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;    /* Allows IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    s = getaddrinfo(argv[1], PORT_NUM, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* Walk through returned list until we find an address structure
     * that can be used to successfully connect a socket */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (cfd == -1) {
            /* On error, try next address */
            continue;
        }

        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            /* Success */
            break;
        }
        /* connect() failed: close this socket and try next address */
        (void) close(cfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not connect socket to any address\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    /* Send requested sequence length, with terminating newline */
    reqlen_str = (argc > 2) ? argv[2] : "1";
    if (write(cfd, reqlen_str, strlen(reqlen_str)) !=
            (ssize_t) strlen(reqlen_str))
    {
        fprintf(stderr, "Partial/failed write (reqlen_str)\n");
        exit(EXIT_FAILURE);
    }
    if (write(cfd, "\n", 1) != 1) {
        fprintf(stderr, "Partial/failed write (newline)\n");
        exit(EXIT_FAILURE);
    }

    /* Read and display sequence number returned by server */
    num_read = read_line(cfd, seqnum_str, INT_LEN);
    if (num_read == -1) {
        perror("read_line");
        exit(EXIT_FAILURE);
    }
    if (num_read == 0) {
        fprintf(stderr, "Unexpected EOF from server\n");
        exit(EXIT_FAILURE);
    }

    printf("Sequence number: %s", seqnum_str);  /* Includes '\n' */

    (void) close(cfd);
    exit(EXIT_SUCCESS);
}
