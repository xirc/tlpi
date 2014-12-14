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
#include <sys/wait.h>


int
main(int argc, char *argv[])
{
    int rc, j;
    struct rlimit rlim;
    int n_preforks, n_forks;

    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [n-preforks] [n-forks]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    n_preforks = (argc > 1) ? atoi(argv[1]) : 15;
    if (n_preforks < 0) {
        fprintf(stderr, "n-preforks %s >= 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    n_forks = (argc > 2) ? atoi(argv[2]) : n_preforks;
    if (n_forks < 0) {
        fprintf(stderr, "n-forks %s >= 0\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    /* prefork */
    for (j = 0; j < n_preforks; ++j) {
        switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            _exit(EXIT_SUCCESS);
        default:
            /* do nothing */
            printf("fork %d\n", j);
            break;
        }
    }

    rlim.rlim_cur = 10; /*  10 process */
    rlim.rlim_max = 20; /*  20 process */
    printf("setrlimit(RLIMIT_NPROC, {soft=%lld, hard=%lld})\n",
            (long long) rlim.rlim_cur, (long long) rlim.rlim_max);
    rc = setrlimit(RLIMIT_NPROC, &rlim);
    if (rc == -1) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    rlim.rlim_cur = 0;
    rlim.rlim_max = 0;
    rc = getrlimit(RLIMIT_NPROC, &rlim);
    if (rc == -1) {
        perror("getrlimit");
        exit(EXIT_FAILURE);
    }
    printf("getrlimit(RLIMIT_NPROC,...) = { soft=%lld, hard=%lld}\n",
            (long long) rlim.rlim_cur, (long long) rlim.rlim_max);

    /* fork */
    for (j = 0; j < n_forks; ++j) {
        switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            _exit(EXIT_SUCCESS);
        default:
            /* do nothing */
            printf("fork %d\n", j);
            break;
        }
    }

    exit(EXIT_SUCCESS);
}
