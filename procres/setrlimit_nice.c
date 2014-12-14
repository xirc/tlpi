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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>


int
main(int argc, char *argv[])
{
    int rc, prio;
    struct rlimit rlim;

    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [prev_prio] [next_prio]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    prio = (argc > 1) ? atoi(argv[1]) : 0;
    printf("setpriority(PRIO_PROCESS, 0, %d)\n", prio);
    rc = setpriority(PRIO_PROCESS, 0, prio);
    if (rc == -1) {
        perror("setpriority");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    prio = getpriority(PRIO_PROCESS, 0);
    if (prio == -1 && errno != 0) {
        perror("getpriority");
        exit(EXIT_FAILURE);
    }
    printf("getpriority(PRIO_PROCESS, 0) = %d\n", prio);

    rlim.rlim_cur = 20;
    rlim.rlim_max = 30;
    printf("setrlimit(RLIMIT_NICE, {soft=%lld, hard=%lld})\n",
            (long long) rlim.rlim_cur, (long long) rlim.rlim_max);
    rc = setrlimit(RLIMIT_NICE, &rlim);
    if (rc == -1) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    rlim.rlim_cur = 0;
    rlim.rlim_max = 0;
    rc = getrlimit(RLIMIT_NICE, &rlim);
    if (rc == -1) {
        perror("getrlimit");
        exit(EXIT_FAILURE);
    }
    printf("getrlimit(RLIMIT_NICE,...) = { soft=%lld, hard=%lld}\n",
            (long long) rlim.rlim_cur, (long long) rlim.rlim_max);

    errno = 0;
    prio = getpriority(PRIO_PROCESS, 0);
    if (prio == -1 && errno != 0) {
        perror("getpriority");
        exit(EXIT_FAILURE);
    }
    printf("getpriority(PRIO_PROCESS, 0) = %d\n", prio);

    prio = (argc > 2) ? atoi(argv[2]) : -15;
    printf("setpriority(PRIO_PROCESS, 0, %d)\n", prio);
    rc = setpriority(PRIO_PROCESS, 0, prio);
    if (rc == -1) {
        perror("setpriority");
        exit(EXIT_FAILURE);
    }

    errno = 0;
    prio = getpriority(PRIO_PROCESS, 0);
    if (prio == -1 && errno != 0) {
        perror("getpriority");
        exit(EXIT_FAILURE);
    }
    printf("getpriority(PRIO_PROCESS, 0) = %d\n", prio);

    exit(EXIT_SUCCESS);
}
