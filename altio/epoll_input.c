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


/* epoll_input.c

   Example of the use of the Linux epoll API.

   Usage: epoll_input file...

   This program opens all of the files named in its command-line arguments
   and monitors the resulting file descriptors for input events.

   This program is Linux (2.6 and later) specific.
*/


#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF    1024     /* Maximum bytes fetched by a single read() */
#define MAX_EVENTS    5     /* Maximum number of events to be returned from
                               a single epoll_wait() call */


int
main(int argc, char *argv[])
{
    int epfd, ready, fd, s, j, num_open_fds;
    struct epoll_event ev;
    struct epoll_event evlist[MAX_EVENTS];
    char buf[MAX_BUF];

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s file...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    epfd = epoll_create(argc - 1);
    if (epfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    /* Open each file on command line, and add it to the "interest list"
     * for the epoll instance */
    for (j = 1; j < argc; ++j) {
        fd = open(argv[j], O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        printf("opened \"%s\" on fd %d\n", argv[j], fd);

        ev.events = EPOLLIN;        /* Only interested in input events */
        ev.data.fd = fd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }
    }

    num_open_fds = argc - 1;
    while (num_open_fds > 0) {
        /* Fetch up to MAX_EVENTS items from the ready list */
        printf("About to epoll_wait()\n");
        ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }
        }
        printf("Ready: %d\n", ready);

        /* Deal with returned list of events */
        for (j = 0; j < ready; ++j) {
            printf("  fd=%d; events: %s%s%s\n",
                evlist[j].data.fd,
                (evlist[j].events & EPOLLIN)  ? "EPOLLIN " : "",
                (evlist[j].events & EPOLLHUP) ? "EPOLLHUP " : "",
                (evlist[j].events & EPOLLERR) ? "EPOLLERR) " : "");

            if (evlist[j].events & EPOLLIN) {
                s = read(evlist[j].data.fd, buf, MAX_BUF);
                if (s == -1) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                printf("    read %d bytes: %.*s\n", s, s, buf);
            } else if (evlist[j].events & (EPOLLHUP | EPOLLERR)) {
                /* After the epoll_wait(), EPOLLIN and EPOLLHUP may
                    * both have been set. But we'll only get here, and
                    * thus close the file descriptor, if EPOLLIN was
                    * not set. This ensures that all outstanding input
                    * (possibly more than MAX_BUF bytes) is consumed
                    * (by further loop iterations) before the file
                    * descriptor is closed. */
                printf("    closing fd %d\n", evlist[j].data.fd);
                if (close(evlist[j].data.fd) == -1) {
                    perror("close");
                    exit(EXIT_FAILURE);
                }
                --num_open_fds;
            }
        }
    }

    printf("All file descriptors closed; bye\n");
    exit(EXIT_SUCCESS);
}
