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
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <signal.h>


static unsigned int
m_alarm(unsigned int seconds)
{
    int retc;
    struct itimerval newval, oldval;
    newval.it_value.tv_sec = seconds;
    newval.it_value.tv_usec = 0;
    newval.it_interval.tv_sec = 0;
    newval.it_interval.tv_usec = 0;

    retc = setitimer(ITIMER_REAL, &newval, &oldval);
    /* this setitimer will not raise error */
    assert(retc != -1);

    return oldval.it_value.tv_sec +
           (oldval.it_value.tv_usec + 99999) / 1000000;
}


static void
handler(int sig __attribute__((unused)))
{
    /* do notthing */
}


static void
test_alarm(const char *msg, unsigned int (*alm)(unsigned int))
{
    struct sigaction sa, old;
    unsigned int remainings;

    printf("Test %p (%s)\n", (void*)alm, msg != NULL ? msg : "?");

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, &old) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* alarm */
    remainings = alm(3);
    assert(remainings == 0);
    remainings = alm(0);
    assert(remainings != 0);
    remainings = alm(3);
    assert(remainings == 0);

    sigset_t waitsigs;
    sigemptyset(&waitsigs);
    sigaddset(&waitsigs, SIGALRM);
    if (sigwaitinfo(&waitsigs, NULL) == -1) {
        perror("sigwaitinfo");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGALRM, &old, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    test_alarm("ALARM", alarm);
    test_alarm("M_ALARM", m_alarm);

    exit(EXIT_SUCCESS);
}
