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
#include <time.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int rc;
    struct sched_param sp;

    sp.sched_priority = 1;
    if (sched_setscheduler(getpid(), SCHED_RR, &sp) == -1) {
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }

    rc = sched_yield();
    if (rc == -1) {
        perror("sched_yield");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
