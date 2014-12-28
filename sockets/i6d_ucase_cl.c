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


/* i6d_ucase_cl.c

   Client for i6d_ucase_sv.c: send each command-line argument as a datagram to
   the server, and then display the contents of the server's response datagram.
*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "i6d_ucase.h"


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "%s host-address msg...\n", progname);
}


int
main(int argc, char *argv[])
{
    struct sockaddr_in6 svaddr;
    int sfd, j;
    size_t msglen;
    ssize_t num_bytes;
    char resp[BUF_SIZE];

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc < 3) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    sfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_in6));
    svaddr.sin6_family = AF_INET6;
    svaddr.sin6_port = htons(PORT_NUM);
    if (inet_pton(AF_INET6, argv[1], &svaddr.sin6_addr) <= 0) {
        fprintf(stderr, "inet_pton failed for address '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Send messages to server; echo responses on stdout */
    for (j = 2; j < argc; ++j) {
        msglen = strlen(argv[j]);
        if (sendto(sfd, argv[j], msglen, 0, (struct sockaddr *) &svaddr,
                    sizeof(struct sockaddr_in6)) != (ssize_t) msglen)
        {
            perror("sendto");
            exit(EXIT_FAILURE);
        }

        num_bytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        if (num_bytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        printf("Response %d: %.*s\n", j - 1, (int) num_bytes, resp);
    }

    exit(EXIT_SUCCESS);
}
