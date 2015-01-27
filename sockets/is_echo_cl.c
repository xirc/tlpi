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


/* is_echo_cl.c

   A simple client to communicate with the standard "echo" server.

   In many Linux distributions, the "echo" server is not started by default.
   Normally it is implemented internally by inetd(8), and to enable we must
   do the following *as root*:

   1.   Edit the file /etc/inetd.conf, and uncomment the two lines
        for the the "echo" service by removing the '#' character at the
        beginning of the line. The lines typically look like this:

                # echo    stream  tcp     nowait  root    internal
                # echo    dgram   udp     wait    root    internal

        and we must change them to this:

                echo    stream  tcp     nowait  root    internal
                echo    dgram   udp     wait    root    internal

   2.   Inform inetd(8) of the change using the following command:

                bash# killall -HUP inetd

   If your system uses xinetd(8) instead of inetd(8), then read the
   xinetd.conf(5) manual page. (You may also find that your system has a GUI
   admin tool that allows you to easily enable/disable the "echo" service.)

   See also is_echo_sv.c.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "inet_sockets.h"

#define BUF_SIZE 128


int
main(int argc, char *argv[])
{
    int sfd;
    ssize_t num_read;
    char buf[BUF_SIZE];

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s host\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sfd = inet_connect(argv[1], "echo", SOCK_STREAM);
    if (sfd == -1) {
        perror("inet_sockets");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:     /* Child: read server's response, echo on stdout */
        while (1) {
            num_read = read(sfd, buf, BUF_SIZE);
            if (num_read <= 0) {        /* Exit on EOF or error */
                break;
            }
            printf("%.*s", (int) num_read, buf);
        }
        exit(EXIT_SUCCESS);

    default:    /* Parent: write contents of stdin to socket */
        while (1) {
            num_read = read(STDIN_FILENO, buf, BUF_SIZE);
            if (num_read <= 0) {        /* Exit loop on EOF or error */
                break;
            }
            if (write(sfd, buf, num_read) != num_read) {
                fprintf(stderr, "write() failed\n");
                exit(EXIT_FAILURE);
            }
        }

        /* Close writing channel, so server sees EOF */
        if (shutdown(sfd, SHUT_WR) == -1) {
            perror("shutdown");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
}
