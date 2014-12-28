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


/* us_xfr_sv.c

   An example UNIX stream socket server. Accepts incoming connections
   and copies data sent from clients to stdout.

   See also us_xfr_cl.c.
*/


#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "us_xfr.h"

#define BACKLOG 5


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct sockaddr_un addr;
    int sfd, cfd;
    ssize_t num_read;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Construct server socket address, bind socket to it,
     * and make this a listening socket */
    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        perror("remove-" SV_SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &addr,
                sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
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
