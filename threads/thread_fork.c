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
#include <sys/wait.h>
#include <signal.h>
#include <string.h>


static void *
forking(void *arg __attribute__((unused)))
{
    struct timespec ts;
    pid_t pid;

    ts.tv_sec = 1;
    ts.tv_nsec = 0;
    while (1) {
        switch (pid = fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0: /* child */
            exit(EXIT_SUCCESS);
        default: /* parent */
            printf("FORK_THREAD: fork %ld\n", (long int) pid);
            nanosleep(&ts, NULL);
        }
    }

    /* cannot reach */
    return NULL;
}


static void *
waiting(void *arg __attribute__((unused)))
{
    int s;
    sigset_t waitmask;
    siginfo_t si;

    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGCHLD);
    sigaddset(&waitmask, SIGINT);
    while (1) {
        printf("WAIT_THREAD: waitsiginfo\n");
        s = sigwaitinfo(&waitmask, &si);
        if (s == -1) {
            perror("sigwaitinfo");
            exit(EXIT_FAILURE);
        }
        printf("WAIT_THREAD: receive sig %d (%s) from pid %ld\n",
                s, strsignal(s), (long int) si.si_pid);
        if (s == SIGINT) {
            exit(EXIT_SUCCESS);
        } else if (s == SIGCHLD) {
            printf("WAIT_THREAD: waitpid %ld\n",
                    (long int) si.si_pid);
            s = waitpid(si.si_pid, NULL, 0);
            if (s == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        }
    }

    return NULL;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int s, i;
    pthread_t ft, wt;
    sigset_t blockmask;

    sigemptyset(&blockmask);
    for (i = 0; i < NSIG; ++i) {
        sigaddset(&blockmask, i);
    }
    s = sigprocmask(SIG_SETMASK, &blockmask, NULL);
    if (s == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    s = pthread_create(&ft, NULL, forking, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    s = pthread_create(&wt, NULL, waiting, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    while (1) {
        pause();
    }

    exit(EXIT_SUCCESS);
}
