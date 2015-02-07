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

#define MAX_EVENTS 5


int
main(int argc, char *argv[])
{
    int epfd;
    struct epoll_event evlist[MAX_EVENTS];
    int timeout_ms;
    int ready;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s {-|timeout}\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-") == 0) {
        timeout_ms = -1;
    } else {
        timeout_ms = strtoul(argv[1], NULL, 0) * 1000;
    }

    epfd = epoll_create(1);
    if (epfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    /* epoll_wait(2) waits? */
    ready = epoll_wait(epfd, evlist, MAX_EVENTS, timeout_ms);
    if (ready == -1) {
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    }
    printf("ready: %d\n", ready);

    exit(EXIT_SUCCESS);
}
