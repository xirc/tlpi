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
#include <time.h>
#include <sys/wait.h>

#include "print_rusage.h"


static void
do_some_work()
{
    int i, acc;

    acc = 0;
    for (i = 0; i < 10000; ++i) {
        acc += i;
        acc *= i;
        acc -= i;
        acc /= 3;
        acc %= i;
    }
    (void) acc;

    sleep(1);
}


int
main(int argc, char *argv[])
{
    pid_t cpid;
    int nforks;
    int j, retc;
    struct timespec ts;
    struct rusage ru;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s nforks\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    nforks = atoi(argv[1]);
    if (nforks < 0) {
        fprintf(stderr, "nforks %s > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Print rusage before forks */
    printf("BEFORE FORK: getrusage\n");
    if (getrusage(RUSAGE_CHILDREN, &ru) == -1) {
        perror("getrusage");
        exit(EXIT_FAILURE);
    }
    print_rusage("", &ru);

    /* forks n childs */
    for (j = 0; j < nforks; ++j) {
        switch (cpid = fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0: /* child */
            do_some_work();
            _exit(EXIT_SUCCESS);
        default: /* parent */
            break;
        }
    }

    /* parent only come here */
    /* Make a child chance to exit */
    ts.tv_sec = nforks / 10;
    ts.tv_nsec = (nforks % 10) * 100000000 /* 100 MS */;
    retc = nanosleep(&ts, NULL);
    if (retc == -1) {
        /* For simpilicity We handle EINTER as error */
        perror("nanosleep");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nforks; ++j) {
        retc = wait(NULL);
        if (retc == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }

    /* Print rusage after forks */
    printf("AFTER  FORK: getrusage\n");
    if (getrusage(RUSAGE_CHILDREN, &ru) == -1) {
        perror("getrusage");
        exit(EXIT_FAILURE);
    }
    print_rusage("", &ru);

    exit(EXIT_SUCCESS);
}
