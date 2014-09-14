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
#include <sched.h>


static void
usage(FILE *fp, char *progname)
{
        fprintf(fp,
                "usage: %s policy priority command [arg]...\n", progname);
        fprintf(fp,
                "          policy: r (SCHED_RR), f (FIFO), o (OTHER)\n");
}


int
main(int argc, char *argv[])
{
    int pol;
    int prio, min_prio, max_prio;
    struct sched_param sp;
    int rc;

    if (argc < 4 ||
        strcmp(argv[1], "--help") == 0 ||
        strchr("rfo", argv[1][0]) == NULL)
    {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Policy */
    pol = argv[1][0] == 'r' ? SCHED_RR :
          argv[1][0] == 'f' ? SCHED_FIFO :
                              SCHED_OTHER;

    /* Priority */
    min_prio = sched_get_priority_min(pol);
    if (min_prio == -1) {
        perror("sched_get_priority_min");
        exit(EXIT_FAILURE);
    }
    max_prio = sched_get_priority_max(pol);
    if (max_prio == -1) {
        perror("sched_get_priority_max");
        exit(EXIT_FAILURE);
    }
    prio = atoi(argv[2]);
    if (prio < min_prio || prio > max_prio) {
        fprintf(stderr, "%d <= priority  <= %d\n", min_prio, max_prio);
        exit(EXIT_FAILURE);
    }

    sp.sched_priority = prio;
    rc = sched_setscheduler(getpid(), pol, &sp);
    if (rc == -1) {
        perror("sched_setpriority");
        exit(EXIT_FAILURE);
    }

    /* drop priviledge */
    rc = setregid(getgid(), getgid());
    if (rc == -1) {
        perror("setregid");
        exit(EXIT_FAILURE);
    }
    rc = setreuid(getuid(), getuid());
    if (rc == -1) {
        perror("setreuid");
        exit(EXIT_FAILURE);
    }

    rc = execvp(argv[3], argv+3);
    if (rc == -1) {
        perror("execve");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
