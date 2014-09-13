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


/* sched_view.c

   Display the scheduling policy and priority for the processes whose PID
   are provided on command line.

   See also sched_set.c.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>


int
main(int argc, char *argv[])
{
    int j, pol;
    struct sched_param sp;
    pid_t pid;

    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [PID...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (j = 1; j < argc; ++j) {
        pid = atol(argv[j]);
        if (pid < 0) {
            fprintf(stderr, "PID %s >= 0\n", argv[j]);
            exit(EXIT_FAILURE);
        }

        pol = sched_getscheduler(pid);
        if (pol == -1) {
            perror("sched_getscheduler");
            exit(EXIT_FAILURE);
        }

        if (sched_getparam(pid, &sp) == -1) {
            perror("sched_getparam");
            exit(EXIT_FAILURE);
        }

        printf("%s: %-5s %2d\n", argv[j],
                (pol == SCHED_OTHER) ? "OTHER" :
                (pol == SCHED_RR) ? "RR" :
                (pol == SCHED_FIFO) ? "FIFO" :
#ifdef SCHED_BATCH      /* Linux-specific */
                (pol == SCHED_BATCH) ? "BATCH" :
#endif
#ifdef SCHED_IDLE       /* Linux-specific */
                (pol == SCHED_IDLE) ? "IDLE" :
#endif
                "???", sp.sched_priority);
    }
    exit(EXIT_SUCCESS);
}
