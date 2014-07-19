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


/* t_nanosleep.c

   Demonstrate the use of nanosleep() to sleep for an interval
   specified in nanoseconds.

   See also t_clock_nanosleep.c.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>


static void
sigint_handler(int sig __attribute__((unused)))
{
    /* do nothing */
}


int
main(int argc, char *argv[])
{
    struct timeval start, finish;
    struct timespec request, remain;
    struct sigaction sa;
    int s;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s secs nanosecs\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    request.tv_sec = atoi(argv[1]);
    if (request.tv_sec < 0) {
        fprintf(stderr, "secs %s >= 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    request.tv_nsec = atoi(argv[2]);
    if (request.tv_nsec < 0) {
        fprintf(stderr, "nanosecs %s >= 0\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    /* Allow SIGINT handler to interrupt nanosleep() */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (gettimeofday(&start, NULL) == -1) {
        perror("gettimeofday");
        exit(EXIT_FAILURE);
    }

    while (1) {
        s = nanosleep(&request, &remain);
        if (s == -1 && errno != EINTR) {
            perror("nanosleep");
            exit(EXIT_FAILURE);
        }

        if (gettimeofday(&finish, NULL) == -1) {
            perror("gettimeofday");
            exit(EXIT_FAILURE);
        }
        printf("Slept for: %9.6f secs\n",
                finish.tv_sec - start.tv_sec +
                (finish.tv_usec - start.tv_usec) * 1e-6);

        /* nanosleep() completed */
        if (s == 0) {
            break;
        }

        printf("Remaining: %2ld.%09ld\n",
                (long) remain.tv_sec, (long) remain.tv_nsec);
        request = remain;
    }

    printf("Sleep complete\n");
    exit(EXIT_SUCCESS);
}
