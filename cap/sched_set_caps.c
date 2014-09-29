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
#include <sys/capability.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <string.h>

#include "caplib.h"


static void
usage(FILE *fp, const char *prog)
{
    fprintf(fp,
            "usage: %s policy priority [pid...]\n"
            "          policy is 'r' (RR), 'f' (FIFO), "
#ifdef SCHED_BATCH      /* Linux-specific */
            "'b' (BATCH), "
#endif
#ifdef SCHED_IDLE       /* Linux-specific */
            "'i' (DILE), "
#endif
            "or 'o' (OTHER)\n", prog);
}


int
main(int argc, char *argv[])
{
    int j, pol;
    struct sched_param sp;
    int prio_min, prio_max;
    pid_t pid;

    if (argc < 3 || strchr("rfo", argv[1][0]) == NULL) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    switch (argv[1][0]) {
        case 'r': pol = SCHED_RR;
                  break;
#ifdef SCHED_BATCH
        case 'b': pol = SCHED_BATCH;
                  break;
#endif
#ifdef SCHED_IDLE
        case 'i': pol = SCHED_IDLE;
                  break;
#endif
        default: pol = SCHED_OTHER;
                 break;
    }

    prio_min = sched_get_priority_min(pol);
    if (prio_min == -1) {
        perror("sched_get_priority_min");
        exit(EXIT_FAILURE);
    }
    prio_max = sched_get_priority_max(pol);
    if (prio_max == -1) {
        perror("sched_get_priority_max");
        exit(EXIT_FAILURE);
    }
    sp.sched_priority = atoi(argv[2]);
    if (sp.sched_priority < prio_min) {
        fprintf(stderr, "invalid priority %s < %d\n", argv[2], prio_min);
        exit(EXIT_FAILURE);
    }
    if (sp.sched_priority > prio_max) {
        fprintf(stderr, "invalid priority %s > %d\n", argv[2], prio_max);
        exit(EXIT_FAILURE);
    }

    for (j = 3; j < argc; ++j) {
        pid = atol(argv[j]);
        if (pid < 0) {
            fprintf(stderr, "PID %s > 0\n", argv[j]);
            exit(EXIT_FAILURE);
        }

        if (raise_cap(CAP_SYS_NICE) == -1) {
            fprintf(stderr, "raise_cap failed\n");
            exit(EXIT_FAILURE);
        }

        if (sched_setscheduler(pid, pol, &sp) == -1) {
            perror("sched_setscheduler");
            exit(EXIT_FAILURE);
        }

        if (drop_cap(CAP_SYS_NICE) == -1) {
            fprintf(stderr, "drop_cap failed\n");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
