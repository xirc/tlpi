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
#include <fcntl.h>

#define MAX_EVENTS 5


int
main(int argc, char *argv[])
{
    int fd1, fd2, fds[2];
    int epfd, j, ready;
    struct epoll_event ev;
    struct epoll_event evlist[MAX_EVENTS];

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s msg\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (pipe(fds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    fd1 = fds[0];

    epfd = epoll_create(1);
    if (epfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = fd1;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd1, &ev) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    /* Suppose that 'fd1' now happens to be come ready for input */
    if (write(fds[1], argv[1], strlen(argv[1])) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    fd2 = dup(fd1);
    (void) close(fd1);
    ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
    if (ready == -1) {
        perror("epoll_wait");
    }

    for (j = 0; j < ready; ++j) {
        printf("  fd=%d; events: %s%s%s\n", evlist[j].data.fd,
                (evlist[j].events & EPOLLIN)  ? "EPOLLIN " : " ",
                (evlist[j].events & EPOLLHUP) ? "EPOLLHUP " : " ",
                (evlist[j].events & EPOLLERR) ? "EPOLLERR " : " ");
    }

    (void) close(fd2);
    (void) close(fds[1]);

    exit(EXIT_SUCCESS);
}
