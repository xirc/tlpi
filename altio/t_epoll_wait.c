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
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_EVENTS 1


int
main(int argc, char *argv[])
{
    int nfds, j, epfd, ready;
    int *fd;
    struct epoll_event ev;
    struct epoll_event evlist[MAX_EVENTS];

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s nfds\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    nfds = strtoul(argv[1], NULL, 0);
    fd = calloc(nfds, sizeof(int));
    if (fd == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nfds; ++j) {
        fd[j] = dup(STDIN_FILENO);
        if (fd[j] == -1) {
            perror("dup");
            exit(EXIT_FAILURE);
        }
    }

    epfd = epoll_create(1);
    if (epfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nfds; ++j) {
        ev.data.fd = fd[j];
        ev.events = EPOLLIN;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd[j], &ev) == -1) {
            perror("epoll_ctl-ADD");
            exit(EXIT_FAILURE);
        }
    }

    while (1) {
        ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
        if (ready == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (j = 0; j < ready; ++j) {
            printf("evlist[%d]; fd: %d %s %s %s\n",
                    j, evlist[j].data.fd,
                    evlist[j].events & EPOLLIN  ? "IN " : " ",
                    evlist[j].events & EPOLLHUP ? "HUP " : " ",
                    evlist[j].events & EPOLLERR ? "ERR " : " ");
        }
        sleep(1);
    }

    exit(EXIT_SUCCESS);
}
