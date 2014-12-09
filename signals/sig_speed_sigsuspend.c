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


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>


#define TESTSIG SIGUSR1


static void
handler(int sig __attribute__((unused)))
{
    /* do nothing */
}


int
main(int argc, char *argv[])
{
    int num_sigs, scnt;
    pid_t cpid;
    sigset_t blocked_mask, empty_mask;
    struct sigaction sa;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s num-sigs\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    num_sigs = atoi(argv[1]);
    if (num_sigs < 0) {
        fprintf(stderr, "num-sigs %s > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(TESTSIG, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Block the signal before fork(), so that the child doesn't manage
       to send it to the parent before the parent is ready to catch it */

    sigemptyset(&blocked_mask);
    sigaddset(&blocked_mask, TESTSIG);
    if (sigprocmask(SIG_SETMASK, &blocked_mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&empty_mask);
    switch (cpid = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:     /* child */
        for (scnt = 0; scnt < num_sigs; scnt++) {
            if (kill(getppid(), TESTSIG) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
            if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
                perror("sigsuspend");
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_SUCCESS);

    default: /* parent */
        for (scnt = 0; scnt < num_sigs; scnt++) {
            if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
                perror("sigsuspend");
                exit(EXIT_FAILURE);
            }
            if (kill(cpid, TESTSIG) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_SUCCESS);
    }
}
