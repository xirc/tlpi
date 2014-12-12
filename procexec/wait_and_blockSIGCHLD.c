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
#include <signal.h>
#include <string.h>
#include <sys/wait.h>


void handler(int sig)
{
    /* UNSAFE */
    printf("handler: %d (%s)\n", sig, strsignal(sig));
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct sigaction sa;
    sigset_t mask;
    int status;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;

    /* Disable buffering for stdout */
    setbuf(stdout, NULL);

    /* Set signalhandler for SIGCHLD */
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction(SIGCHLD,...)");
        exit(EXIT_FAILURE);
    }

    /* Block signal SIGCHLD */
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask(SIG_BLOCK, SIGCHLD, ...)");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0: /* child */
        /* do nothing */
        sleep(3);
        printf("EXIT child\n");
        exit(EXIT_SUCCESS);

    default: /* parent */
        break;
    }

    /* parent carries on to wait child */
    printf("BEGIN wait(...)\n");
    if (wait(&status) == -1) {
        perror("wait");
        exit(EXIT_FAILURE);
    }
    printf("END wait(...)\n");

    /* Unblock signal SIGCHLD */
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        perror("sigprocmask(SIG_UNBLOCK, SIGCHLD, ...)");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
