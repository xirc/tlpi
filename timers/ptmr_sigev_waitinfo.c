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

#include "itimerspec_from_string.h"


#define TIMER_SIG SIGRTMAX
#define BUF_SIZE 1024


static char*
now(const char *format)
{
    static char buf[BUF_SIZE];
    time_t t;
    size_t s;
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);
    if (tm == NULL) {
        return NULL;
    }
    s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);
    return (s == 0) ? NULL : buf;
}


int
main(int argc, char *argv[])
{
    struct itimerspec ts;
    sigset_t wsigs;
    siginfo_t sinfo;
    timer_t *tidptr;
    struct sigevent sev;
    timer_t *tidlist;
    int j;

    if (argc < 2) {
        fprintf(stderr,
                "usage: %s secs[/nsecs][:int-secs[/int-nsecs]]...\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    tidlist = calloc(argc -1, sizeof(timer_t));
    if (tidlist == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Block notification signal for sigwaitinfo */
    sigemptyset(&wsigs);
    sigaddset(&wsigs, TIMER_SIG);
    if (sigprocmask(SIG_BLOCK, &wsigs, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    /* Create and start one timer for each command-line argument */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = TIMER_SIG;
    for (j = 0; j < argc - 1; ++j) {
        itimerspec_from_string(argv[j+1], &ts);
        sev.sigev_value.sival_ptr = &tidlist[j];
            /* Allow handler to get ID of this timer */
        if (timer_create(CLOCK_REALTIME, &sev, &tidlist[j]) == -1) {
            perror("timer_create");
            exit(EXIT_FAILURE);
        }
        printf("Timer ID: %ld (%s)\n", (long) tidlist[j], argv[j+1]);

        if (timer_settime(tidlist[j], 0, &ts, NULL) == -1) {
            perror("timer_settime");
            exit(EXIT_FAILURE);
        }
    }

    /* Wait for incoming timer signals */
    while (1) {
        if (sigwaitinfo(&wsigs, &sinfo) == -1 && errno != EINTR) {
            perror("sigwaitinfo");
            exit(EXIT_FAILURE);
        }
        if (errno == EINTR) continue;

        tidptr = sinfo.si_value.sival_ptr;
        printf("[%s] Got signal %d\n", now("%T"), sinfo.si_signo);
        printf("    *sival_ptr         = %ld\n", (long) *tidptr);
        printf("    timer_getoverrun() = %d\n",
                timer_getoverrun(*tidptr));
    }
}
