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
#include <sched.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pid_t pid;
    struct timespec ts;
    struct sched_param sp;

    pid = getpid();
    sp.sched_priority = 1;
    if (sched_setscheduler(pid, SCHED_RR, &sp) == -1) {
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }

    if (sched_rr_get_interval(pid, &ts) == -1) {
        perror("sched_rr_get_interval");
        exit(EXIT_FAILURE);
    }

    printf("Time slice = %.6f\n", ts.tv_sec + ts.tv_nsec * 1e-9);

    exit(EXIT_SUCCESS);
}
