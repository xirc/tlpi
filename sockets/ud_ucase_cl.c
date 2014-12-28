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


/* ud_ucase_cl.c

   A UNIX domain client that communicates with the server in ud_ucase_sv.c.
   This client sends each command-line argument as a datagram to the server,
   and then displays the contents of the server's response datagram.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <ctype.h>

#include "ud_ucase.h"


int
main(int argc, char *argv[])
{
    struct sockaddr_un svaddr, claddr;
    int sfd, j;
    size_t msglen;
    ssize_t num_bytes;
    char resp[BUF_SIZE];

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s msg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Create client socket; bind to unique pathname (based on PID) */
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path, sizeof(claddr.sun_path),
            "/tmp/ud_ucase_cl.%ld", (long) getpid());

    if (bind(sfd, (struct sockaddr *) &claddr,
                sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Construct address of server */
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    /* Send messages to server; echo responses on stdout */
    for (j = 1; j < argc; ++j) {
        /* May be longer than BUF_SIZE */
        msglen = strlen(argv[j]);

        if (sendto(sfd, argv[j], msglen, 0, (struct sockaddr *) &svaddr,
                    sizeof(struct sockaddr_un)) != (ssize_t)msglen)
        {
            fprintf(stderr, "sendto: errno=%d\n", errno);
            exit(EXIT_FAILURE);
        }

        num_bytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        if (num_bytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("Response %d: %.*s\n", j, (int) num_bytes, resp);
    }

    /* Remove client socket pathname */
    (void) remove(claddr.sun_path);
    exit(EXIT_SUCCESS);
}
