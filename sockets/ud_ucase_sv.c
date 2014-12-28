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


/* ud_ucase_sv.c

   A server that uses a UNIX domain datagram socket to receive datagrams,
   convert their contents to uppercase, and then return them to the senders.

   See also ud_ucase_cl.c.
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
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct sockaddr_un svaddr, claddr;
    int sfd, j;
    ssize_t num_bytes;
    socklen_t len;
    char buf[BUF_SIZE];

    /* Create server socket */
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Construct well-known address and bind server socket to it */
    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        perror("remove-" SV_SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &svaddr,
                sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Receive message, convert to uppercase, and return to client */
    while (1) {
        len = sizeof(struct sockaddr_un);
        num_bytes = recvfrom(sfd, buf, BUF_SIZE, 0,
                (struct sockaddr *) &claddr, &len);
        if (num_bytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        printf("Server received %ld bytes from %s\n",
                (long) num_bytes, claddr.sun_path);

        for (j = 0 ; j < num_bytes; ++j) {
            buf[j] = toupper((unsigned char) buf[j]);
        }

        if (sendto(sfd, buf, num_bytes, 0,
                    (struct sockaddr *) &claddr, len) != num_bytes)
        {
            fprintf(stderr, "sendto: errno=%d\n", errno);
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
