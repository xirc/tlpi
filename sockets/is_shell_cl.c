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
#include "is_shell.h"

#define BUF_SIZE 8096


int
main(int argc, char *argv[])
{
    int sfd;
    char *host;
    char *command;
    int retc, num_read;
    char buf[BUF_SIZE];

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s HOSTNAME COMMAND\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    host = argv[1];
    command = argv[2];

    sfd = inet_connect(host, SHELL_SERVICE, SOCK_STREAM);
    if (sfd == -1) {
        perror("inet_connect");
        exit(EXIT_FAILURE);
    }

    retc = send(sfd, command, strlen(command), 0);
    if (retc == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    retc = shutdown(sfd, SHUT_WR);
    if (retc == -1) {
        perror("shutdown");
        exit(EXIT_FAILURE);
    }

    while (1) {
        num_read = read(sfd, buf, BUF_SIZE);
        if (num_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (num_read == 0) {
            break;
        }
        printf("%.*s", BUF_SIZE, buf);
    }

    exit(EXIT_SUCCESS);
}
