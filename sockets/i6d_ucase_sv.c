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


/* i6d_ucase_sv.c

   A server that receives datagrams, converts their contents to uppercase, and
   then returns them to the senders.

   See also i6d_ucase_cl.c.
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
#include <ctype.h>

#include "i6d_ucase.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct sockaddr_in6 svaddr, claddr;
    int sfd, j;
    ssize_t num_bytes;
    socklen_t len;
    char buf[BUF_SIZE];
    char claddr_str[INET6_ADDRSTRLEN];

    sfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_in6));
    svaddr.sin6_family = AF_INET6;
    svaddr.sin6_addr = in6addr_any;     /* Wildcard address */
    svaddr.sin6_port = htons(PORT_NUM);

    if (bind(sfd, (struct sockaddr *) &svaddr,
                sizeof(struct sockaddr_in6)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Receive messages, convert to uppercase, and return to client */
    while (1) {
        len = sizeof(struct sockaddr_in6);
        num_bytes = recvfrom(sfd, buf, BUF_SIZE, 0,
                (struct sockaddr *) &claddr, &len);
        if (num_bytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        if (inet_ntop(AF_INET6, &claddr.sin6_addr,
                    claddr_str, INET6_ADDRSTRLEN) == NULL)
        {
            printf("Couldn't convert client address to string\n");
        } else {
            printf("Server received %ld bytes from (%s, %u)\n",
                    (long) num_bytes, claddr_str, ntohs(claddr.sin6_port));
        }

        for (j = 0; j < num_bytes; ++j) {
            buf[j] = toupper((unsigned char) buf[j]);
        }

        if (sendto(sfd, buf, num_bytes, 0,
                    (struct sockaddr *) &claddr, len) != num_bytes)
        {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
