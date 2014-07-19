/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


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
    struct timespec start, finish;
    struct timespec request;
    time_t secs;
    long nanosecs;
    struct sigaction sa;
    int s;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s secs nanosecs\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    secs = atoi(argv[1]);
    if (request.tv_sec < 0) {
        fprintf(stderr, "secs %s >= 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    nanosecs = atoi(argv[2]);
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

    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    request = start;
    request.tv_sec += secs;
    request.tv_nsec += nanosecs;
    while (1) {
        s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,
                &request, NULL);
        if (s == -1 && errno != EINTR) {
            perror("clock_nanosleep");
            exit(EXIT_FAILURE);
        }

        if (clock_gettime(CLOCK_REALTIME, &finish) == -1) {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }
        printf("Slept for: %9.6f secs\n",
                finish.tv_sec - start.tv_sec +
                (finish.tv_nsec - start.tv_nsec) * 1e-9);

        /* nanosleep() completed */
        if (s == 0) {
            break;
        }

        printf("Remaining: %9.6f secs\n",
                (request.tv_sec - finish.tv_sec) +
                (request.tv_nsec - finish.tv_nsec) * 1e-9);
    }

    printf("Sleep complete\n");
    exit(EXIT_SUCCESS);
}
