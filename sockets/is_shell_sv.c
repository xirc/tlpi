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
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "inet_sockets.h"
#include "is_shell.h"

#define BACKLOG 5
#define BUF_SIZE 8092


static void
sigchld_handler(int sig __attribute__((unused)))
{
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
}


static void
serve(int cfd)
{
    int retc;
    char buf[BUF_SIZE];
    ssize_t num_read;

    retc = 0;
    retc |= dup2(cfd, STDOUT_FILENO);
    retc |= dup2(cfd, STDERR_FILENO);
    if (retc == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    num_read = read(cfd, buf, BUF_SIZE);
    if (num_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    if (num_read == BUF_SIZE - 1 &&
        buf[BUF_SIZE-1] != '\0')
    {
        /* TOO LARGE COMMAND */
        exit(EXIT_FAILURE);
    }

    retc = system(buf);
    if (retc == -1) {
        exit(EXIT_FAILURE);
    }
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int sfd, cfd;
    int retc;
    socklen_t addrlen;
    struct sigaction sa;

    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sigchld_handler;
    retc = sigaction(SIGCHLD, &sa, NULL);
    if (retc == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sfd = inet_listen(SHELL_SERVICE, BACKLOG, &addrlen);
    if (sfd == -1) {
        perror("inet_listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
            break;
        case 0:
            close(sfd);
            serve(cfd);
            _exit(EXIT_SUCCESS);
            break;
        default:
            close(cfd);
            break;
        }
    }

    exit(EXIT_SUCCESS);
}
