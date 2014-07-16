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
#include <signal.h>
#include <string.h>

#include "signal_functions.h"


static void
handler(int sig)
{
    /* UNSAFE */
    printf("SIGNAL RECEIVED %d (%s)\n", sig, strsignal(sig));
}


int
main(int argc, char *argv[])
{
    int sleep_time;
    struct sigaction sa;
    sigset_t block_mask;

    sleep_time = (argc > 1) ? atoi(argv[1]) : 5;
    if (sleep_time <= 0) {
        fprintf(stderr, "sleep-time %s > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGCONT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCONT);
    if (sigprocmask(SIG_BLOCK, &block_mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
    printf("set signal mask SIGCONT\n");
    print_sigmask(stdout, "SIGNAL MASK:\n");

    printf("sleep %ds.\n", sleep_time);
    printf("type <Ctrl-Z>. Then, type 'kill -CONT %ld\n", (long) getpid());
    sleep(sleep_time);
    printf("sleep finished.\n");

    print_sigmask(stdout, "SIGNAL MASK:\n");
    print_pending_sigs(stdout, "PENDINGS:\n");
    printf("unset signal mask SIGCONT\n");
    if (sigprocmask(SIG_UNBLOCK, &block_mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
