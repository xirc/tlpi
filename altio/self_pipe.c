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


#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>


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
    fd_set readfds;
    int ready, nfds, flags;
    struct timeval timeout;
    struct timeval *pto;
    struct sigaction sa;
    char ch;
    int fd, j;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Initialize 'timeout', 'readfds', and 'nfds' for select() */

    if (strcmp(argv[1], "-") == 0) {
        /* Infinite timeout */
        pto = NULL;
    } else {
        pto = &timeout;
        timeout.tv_sec = strtoul(argv[1], NULL, 0);
        timeout.tv_usec = 0;            /* No microseconds */
    }

    nfds = 0;

    /* Build the 'readfds' from the fd numbers given in command line */

    FD_ZERO(&readfds);
    for (j = 2; j < argc; j++) {
        fd = strtoul(argv[j], NULL, 0);
        if (fd >= FD_SETSIZE) {
            fprintf(stderr,
                    "file descriptor exceeds limit (%d)\n", FD_SETSIZE);
            exit(EXIT_FAILURE);
        }
        if (fd >= nfds) {
            /* Record maximum fd + 1 */
            nfds = fd + 1;
        }
        FD_SET(fd, &readfds);
    }

    /* Create pipe before establishing signal handler to prevent race */

    if (pipe(pfd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    /* Add read end of pipe to 'readfds' */
    FD_SET(pfd[0], &readfds);
    /* And adjust 'nfds' if required */
    nfds = (nfds > pfd[0] + 1) ? nfds : pfd[0] + 1;

    /* Make read and write ends of pipe nonblocking */

    flags = fcntl(pfd[0], F_GETFL);
    if (flags == -1) {
        perror("fcntl-F_GETFL");
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;                /* Make read end nonblocking */
    if (fcntl(pfd[0], F_SETFL, flags) == -1) {
        perror("fcntl-F_SETFL");
        exit(EXIT_FAILURE);
    }

    flags = fcntl(pfd[1], F_GETFL);
    if (flags == -1) {
        perror("fcntl-F_GETFL");
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;                /* Make write end nonblocking */
    if (fcntl(pfd[1], F_SETFL, flags) == -1) {
        perror("fcntl-F_SETFL");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;           /* Restart interrupted reads()s */
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
    }

    while ((ready = select(nfds, &readfds, NULL, NULL, pto)) == -1 &&
            errno == EINTR)
    {
        /* Restart if interrupted by signal */
        continue;
    }
    if (ready == -1) {                    /* Unexpected error */
        perror("select");
        exit(EXIT_FAILURE);
    }

    if (FD_ISSET(pfd[0], &readfds)) {    /* Handler was called */
        printf("A signal was caught\n");

        while (1) {                      /* Consume bytes from pipe */
            if (read(pfd[0], &ch, 1) == -1) {
                if (errno == EAGAIN) {
                    break;              /* No more bytes */
                } else {
                    perror("read");     /* Some other error */
                    exit(EXIT_FAILURE);
                }
            }
            /* Perform any actions that should be
             * taken in response to signal */
        }
    }

    /* Examine file descriptor sets returned by select() to see
       which other file descriptors are ready */

    printf("ready = %d\n", ready);
    for (j = 2; j < argc; j++) {
        fd = strtoul(argv[j], NULL, 0);
        printf("%d: %s\n", fd, FD_ISSET(fd, &readfds) ? "r" : "");
    }

    /* And check if read end of pipe is ready */

    printf("%d: %s   (read end of pipe)\n", pfd[0],
            FD_ISSET(pfd[0], &readfds) ? "r" : "");

    if (pto != NULL) {
        printf("timeout after select(): %ld.%03ld\n",
               (long) timeout.tv_sec, (long) timeout.tv_usec / 1000);
    }

    exit(EXIT_SUCCESS);
}
