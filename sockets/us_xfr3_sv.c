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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "unix_sockets.h"
#include "us_xfr3.h"

#define BACKLOG 5


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int sfd, cfd;
    ssize_t num_read;
    char buf[BUF_SIZE];

    sfd = unix_listen(SV_SOCK_PATH, BACKLOG);
    if (sfd == -1) {
        perror("unix_listen");
        exit(EXIT_FAILURE);
    }

    /* Handle client connections iteratively */
    while (1) {
        /* Accept a connection. The connection is returned on a new
         * socket, 'cfd'; the listening socket ('sfd') remains open and
         * can be used to accept further connections.
         */
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) {
            perror("accpet");
            exit(EXIT_FAILURE);
        }

        /* Transfer data from connected socket to stdout until EOF */
        while ((num_read = read(cfd, buf, BUF_SIZE)) > 0) {
            if (write(STDOUT_FILENO, buf, num_read) != num_read) {
                fprintf(stderr, "partial/failed write\n");
                exit(EXIT_FAILURE);
            }
        }
        if (num_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (close(cfd) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
