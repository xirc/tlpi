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
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static int
m_pipe2(int fd[2])
{
    int retc;
    int sv[2];
        /* sv[0]: raad side, sv[1]: write side */

    retc = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    if (retc == -1) {
        return -1;
    }

    retc = shutdown(sv[0], SHUT_WR);
    if (retc == -1) {
        return -1;
    }

    retc = shutdown(sv[1], SHUT_RD);
    if (retc == -1) {
        return -1;
    }

    fd[0] = sv[0];
    fd[1] = sv[1];
    return 0;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int fd[2];
    int retc;
    char buf[4];

    retc = m_pipe2(fd);
    if (retc == -1) {
        perror("m_pipe2");
        exit(EXIT_FAILURE);
    }

    printf("Write data('ABC') to PIPE[1]\n");
    if (write(fd[1], "ABC", 4) != 4) {
        fprintf(stderr, "Cannot write whole buffer\n");
        exit(EXIT_FAILURE);
    }

    printf("Read data from PIPE[0]\n");
    if (read(fd[0], buf, 4) != 4) {
        fprintf(stderr, "Cannot read whole buffer\n");
        exit(EXIT_FAILURE);
    }
    printf("Received: '%s'\n", buf);

    exit(EXIT_SUCCESS);
}
