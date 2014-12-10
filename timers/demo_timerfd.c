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


/* demo_timerfd.c

   Demonstrate the use of the timerfd API, which creates timers whose
   expirations can be read via a file descriptor.

   This program is Linux-specific. The timerfd API is supported since kernel
   2.6.25. Library support is provided since glibc 2.8.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timerfd.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#include "itimerspec_from_string.h"


int
main(int argc, char *argv[])
{
    struct itimerspec ts;
    struct timespec start, now;
    int fd, secs, nanosecs;
    uint64_t max_exp, num_exp, total_exp;
    ssize_t s;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr,
                "usage: %s secs[/nsecs][:int-secs[/int-nsecs]] [max-exp]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    itimerspec_from_string(argv[1], &ts);
    max_exp = (argc > 2) ? strtoul(argv[2], NULL, 0) : 1;
    if (max_exp <= 0) {
        fprintf(stderr, "max-exp %s > 0\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == -1) {
        perror("timerfd_create");
        exit(EXIT_FAILURE);
    }

    if (timerfd_settime(fd, 0, &ts, NULL) == -1) {
        perror("timerfd_settime");
        exit(EXIT_FAILURE);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    for (total_exp = 0; total_exp < max_exp; /* do nothing */) {
        /* Read number of expirations on the timer, and then display
         * time elapsed since timer was started, followed by number
         * of expirations read and total expirations so far. */
        s = read(fd, &num_exp, sizeof(uint64_t));
        if (s != sizeof(uint64_t)) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        total_exp += num_exp;

        if (clock_gettime(CLOCK_MONOTONIC, &now) == -1) {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }

        secs = now.tv_sec - start.tv_sec;
        nanosecs = now.tv_nsec - start.tv_nsec;
        if (nanosecs < 0) {
            secs--;
            nanosecs += 1000000000;
        }

        printf("%d.%03d: expirations read: %llu; total=%llu\n",
                secs, (nanosecs + 500000) / 1000000,
                (unsigned long long) num_exp,
                (unsigned long long) total_exp);
    }

    exit(EXIT_SUCCESS);
}
