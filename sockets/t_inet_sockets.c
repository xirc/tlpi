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

#include "inet_sockets.h"


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "Usage: %s host port\n", progname);
}


int
main(int argc, char *argv[])
{
    int udp_fd;
    int sfd_tcp, cfd_tcp;
    char *host;
    char *service;
    socklen_t addrlen;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }
    host = argv[1];
    service = argv[2];

    /* UDP */
    udp_fd = inet_bind("10000", SOCK_DGRAM, &addrlen);
    if (udp_fd == -1) {
        perror("inet_bind");
        exit(EXIT_FAILURE);
    }
    (void) close(udp_fd);

    /* TCP */
    sfd_tcp = inet_listen(service, 100, &addrlen);
    if (sfd_tcp == -1) {
        perror("inet_listen");
        exit(EXIT_FAILURE);
    }
    cfd_tcp = inet_connect(host, service, SOCK_STREAM);
    if (cfd_tcp == -1) {
        perror("inet_connect");
        exit(EXIT_FAILURE);
    }
    (void) close(cfd_tcp);
    (void) close(sfd_tcp);

    exit(EXIT_SUCCESS);
}
