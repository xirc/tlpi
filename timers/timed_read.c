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


/* timed_read.c

   Demonstrate the use of a timer to place a timeout on a blocking system call
   (read(2) in this case).
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define BUF_SIZE 256


/* SIGALRM handler: interrupts blocked system call */
static void
handler(int sig __attribute__((unused)))
{
    /* UNSAFE (see section 21.1.2) */
    printf("Caught signal\n");
}


int
main(int argc, char *argv[])
{
    struct sigaction sa;
    char buf[BUF_SIZE];
    ssize_t num_read;
    int saved_errno, alarm_time;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [num-secs [restart-flag]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Set up handler for SIGALRM. Allow system calls to be interrupted,
     * unless second command-line argument was supplied */
    sa.sa_flags = (argc > 2) ? SA_RESTART : 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    alarm_time = (argc > 1) ? atoi(argv[1]) : 10;
    if (alarm_time < 0) {
        fprintf(stderr, "num-secs %s > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* This code has a race condition problem.
     * If timeout occured between alarm() and read(),
     * read() was not interrupted by signal handler. */
    alarm(alarm_time);
    num_read = read(STDIN_FILENO, buf, BUF_SIZE - 1);
    saved_errno = errno;
    alarm(0);
    errno = saved_errno;

    /* Determine result of read() */
    if (num_read == -1) {
        if (errno == EINTR) {
            printf("Read timed out\n");
        } else {
            perror("read");
        }
    } else {
        printf("Successful read (%ld bytes): %.*s",
                (long) num_read, (int) num_read, buf);
    }

    exit(EXIT_SUCCESS);
}
