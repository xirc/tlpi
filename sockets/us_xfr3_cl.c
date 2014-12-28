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

#include "unix_sockets.h"
#include "us_xfr3.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int sfd;
    ssize_t num_read;
    char buf[BUF_SIZE];

    sfd = unix_connect(SV_SOCK_PATH, SOCK_STREAM);
    if (sfd == -1) {
        perror("unix_connect");
        exit(EXIT_FAILURE);
    }

    /* Copy stdin to socket */
    while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        if (write(sfd, buf, num_read) != num_read) {
            fprintf(stderr, "partial/failed write\n");
            exit(EXIT_FAILURE);
        }
    }
    if (num_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    /* Closes our socket; server sees EOF */
    exit(EXIT_SUCCESS);
}
