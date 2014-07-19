/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>


static void
handler(int sig __attribute__((unused)))
{
    /* UNSAFE */
    printf("Handle sig %d (%s)\n", sig, strsignal(sig));
}


int
main(int argc, char *argv[])
{
    struct sigevent sev, *evp;
    struct itimerspec ts;
    timer_t tid;
    struct sigaction sa;
    int is_use_null;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [use-null-flag]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    is_use_null = (argc > 1);

    if (is_use_null) {
        printf("Use timer_create(*, NULL, *)\n");
    } else {
        printf("Use timer_create(*, not NULL, *)\n");
    }

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (is_use_null) {
        evp = NULL;
    } else {
        sev.sigev_notify = SIGEV_SIGNAL;
        sev.sigev_signo = SIGALRM;
        evp = &sev;
    }
    if (timer_create(CLOCK_REALTIME, evp, &tid) == -1)
    {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    ts.it_value.tv_sec = 1;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 1;
    ts.it_interval.tv_nsec = 0;
    if (timer_settime(tid, 0, &ts, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    while (1) {
        pause();
    }
}
