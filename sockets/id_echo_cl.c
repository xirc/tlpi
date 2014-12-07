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


/* id_echo_cl.c

   A client for the UDP "echo" service. This program sends each of its
   command-line arguments as a datagram to the server and echoes the
   contents of the datagrams that the server sends in response.

   See also id_echo_sv.c.
*/

#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "id_echo.h"
#include "inet_sockets.h"


int
main(int argc, char *argv[])
{
    int j, sfd;
    size_t len;
    ssize_t num_write, num_read;
    char buf[BUF_SIZE];

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s host msg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Construct server address from first command-line argument */
    sfd = inet_connect(argv[1], SERVICE, SOCK_DGRAM);
    if (sfd == -1) {
        perror("inet_connect");
        exit(EXIT_FAILURE);
    }

    /* Send remaining command-line argument to server as separate datagrams */
    for (j = 2; j < argc; ++j) {
        len = strlen(argv[j]);
        num_write = write(sfd, argv[j], len);
        if (num_write == -1 || (size_t)num_write != len) {
            fprintf(stderr, "partial/failed write\n");
            exit(EXIT_FAILURE);
        }

        num_read = read(sfd, buf, BUF_SIZE);
        if (num_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("[%ld bytes] %.*s\n", (long) num_read, (int) num_read, buf);
    }

    exit(EXIT_SUCCESS);
}
