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
#include <pthread.h>
#include <signal.h>
#include <string.h>


static void *
thread_func(void *arg __attribute__((unused)))
{
    sigset_t pendings;
    int i, s;

    /* wait that parent send signal */
    sleep(1);

    /* get pending signals */
    sigemptyset(&pendings);
    s = sigpending(&pendings);
    if (s != 0) {
        perror("sigpending");
    }

    /* print pending signals */
    printf("thread: %ld\n", (long int) pthread_self());
    for (i = 1; i < NSIG; ++i) {
        s = sigismember(&pendings, i);
        if (s == 1) {
            printf("    %s (%d)\n", strsignal(i), i);
        } else if (s != 0) {
            perror("sigismember");
            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int s;
    pthread_t t1, t2;
    sigset_t block_mask;

    /* block all signals */
    sigemptyset(&block_mask);
    for (int i = 0; i < NSIG; ++i) {
        sigaddset(&block_mask, i);
    }
    s = sigprocmask(SIG_SETMASK, &block_mask, NULL);
    if (s != 0) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    /* create threads */
    s = pthread_create(&t1, NULL, thread_func, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    s = pthread_create(&t2, NULL, thread_func, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    /* send different signals to different threads */
    printf("Main thread: send signal %s (%d) to %ld\n",
            strsignal(SIGUSR1), SIGUSR1, (long int)t1);
    s = pthread_kill(t1, SIGUSR1);
    if (s != 0) {
        errno = s;
        perror("pthread_kill");
        exit(EXIT_FAILURE);
    }
    printf("Main thread: send signal %s (%d) to %ld\n",
            strsignal(SIGUSR2), SIGUSR2, (long int)t2);
    s = pthread_kill(t2, SIGUSR2);
    if (s != 0) {
        errno = s;
        perror("pthread_kill");
        exit(EXIT_FAILURE);
    }

    /* join */
    s = pthread_join(t1, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }
    s = pthread_join(t2, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
