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


/*  self_pipe.c

   Employ the self-pipe trick so that we can avoid race conditions
   while both selecting on a set of file descriptors and
   also waiting for a signal.

   Usage as shown in synopsis below; for example:

        self_pipe - 0
*/


#define _GNU_SOURCE
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>


/* File descriptors for pipe */
static int pfd[2];


static void
handler(int sig __attribute__((unused)))
{
    int savedErrno;
        /* In case we change 'errno' */

    savedErrno = errno;
    if (write(pfd[1], "x", 1) == -1 && errno != EAGAIN) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    errno = savedErrno;
}


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp,
        "usage: %s {timeout|-} fd...\n"
        "\t\t('-' means infinite timeout)\n", progname);
    fprintf(fp,
        "\te.g.\n"
        "\t\t%s - 0\n", progname);
}


int
main(int argc, char *argv[])
{
    int ready, nfds;
    struct sigaction sa;
    char ch;
    int fd, j;
    int timeout_ms;
    struct pollfd *fds;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Initialize 'timeout', 'nfds', and 'fds' for poll() */

    if (strcmp(argv[1], "-") == 0) {
        /* Infinite timeout */
        timeout_ms = -1;
    } else {
        timeout_ms = strtoul(argv[1], NULL, 0) * 1000;
    }

    /* Build the 'fds' from the fd numbers given in command line */

    nfds = argc - 2 + 1;
    fds = calloc(nfds, sizeof(struct pollfd));
    if (fds == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    for (j = 2; j < argc; j++) {
        fd = strtoul(argv[j], NULL, 0);
        fds[j-2].fd = fd;
        fds[j-2].events = POLLIN;
    }

    /* Create pipe before establishing signal handler to prevent race */
    /* Make read and write ends of pipe nonblocking */
    if (pipe2(pfd, O_NONBLOCK) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    /* Add read end of pipe to 'fds' */
    fds[nfds-1].fd = pfd[0];
    fds[nfds-1].events = POLLIN;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;           /* Restart interrupted reads()s */
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
    }

    while ((ready = poll(fds, nfds, timeout_ms)) == -1 &&
            errno == EINTR)
    {
        /* Restart if interrupted by signal */
        continue;
    }
    if (ready == -1) {
        /* Unexpected error */
        perror("poll");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nfds; ++j) {
        if (fds[j].fd == pfd[0] &&
            fds[j].revents & POLLIN)
        {
            /* Handler was called */
            printf("A signal was caught\n");
            /* Consume bytes from pipe */
            while (1) {
                if (read(pfd[0], &ch, 1) == -1) {
                    if (errno == EAGAIN) {
                        /* No more bytes */
                        break;
                    } else {
                        /* Some other error */
                        perror("read");
                        exit(EXIT_FAILURE);
                    }
                }
                /* Perform any actions that should be
                * taken in response to signal */
            }
            break;
        }
    }

    printf("ready = %d\n", ready);
    for (j = 0; j < nfds; ++j) {
        printf("%d: %s%s\n", fds[j].fd,
            fds[j].revents & POLLIN ? "r" : "",
            fds[j].fd == pfd[0] ? "   (read end of pipe)" : "");
    }

    exit(EXIT_SUCCESS);
}
