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


/* rlimit_nproc.c

   Experiment with maximum processes resource limit.

   Usage: rlimit_nproc hard-limit soft-limit

   NOTE: Only Linux and the BSDs support the RLIMIT_NPROC resource limit.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>

#include "print_rlimit.h"


int
main(int argc, char *argv[])
{
    struct rlimit rl;
    int j;
    pid_t child_pid;

    if (argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s soft-limit [hard-limit]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    print_rlimit("Initial maximum process limits: ", RLIMIT_NPROC);

    /* Set new process limits (hard == soft if not specified) */
    rl.rlim_cur = (argv[1][0] == 'i') ? RLIM_INFINITY :
                    strtoul(argv[1], NULL, 0);
    rl.rlim_max = (argc == 2) ? rl.rlim_cur :
                  (argv[2][0] == 'i') ? RLIM_INFINITY :
                    strtoul(argv[1], NULL, 0);

    if (setrlimit(RLIMIT_NPROC, &rl) == -1) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    print_rlimit("New maximum process limits:     ", RLIMIT_NPROC);

    /* Create as many childre as possible */
    for (j = 1; /* infinite */; ++j) {
        switch (child_pid = fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            _exit(EXIT_SUCCESS);
        default:
            /* Parent: display message about each new child and
             * let the resulting zombies accumulate */
            printf("Child %d (PID=%ld) started\n", j, (long) child_pid);
            break;
        }
    }
}
