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

#include "signal_functions.h"


/* counts deliveries of each signal */
static int sig_cnt[NSIG];

/* set nonzero if SIGINT is delivered */
static volatile sig_atomic_t got_sigint = 0;


static void
handler(int sig)
{
    if (sig == SIGINT) {
        got_sigint = 1;
    } else {
        sig_cnt[sig]++;
    }
}


int
main(int argc, char *argv[])
{
    int n, num_secs;
    sigset_t pending_mask, blocking_mask, empty_mask;
    struct sigaction act;

    printf("%s: PID is %ld\n", argv[0], (long) getpid());
    sigemptyset(&empty_mask);
    act.sa_handler = handler;
    act.sa_mask = empty_mask;
    act.sa_flags = 0;
    for (n = 1; n < NSIG; ++n) {
        (void) sigaction(n, &act, NULL);
    }

    /* If a sleep time was specified, temporarily block all signals,
     * sleep (while another process sends us signals), and then
     * display the mask of pending signals and unblock all signals */
    if (argc > 1) {
        num_secs = atoi(argv[1]);
        if (num_secs < 0) {
            fprintf(stderr, "num-secs %s > 0\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        sigfillset(&blocking_mask);
        if (sigprocmask(SIG_SETMASK, &blocking_mask, NULL) == -1) {
            perror("sigprocmask");
            exit(EXIT_FAILURE);
        }

        printf("%s: sleeping for %d seconds\n", argv[0], num_secs);
        sleep(num_secs);

        if (sigpending(&pending_mask) == -1) {
            perror("sigpending");
            exit(EXIT_FAILURE);
        }

        printf("%s: pending signals are: \n", argv[0]);
        print_sigset(stdout, "\t\t", &pending_mask);

        sigemptyset(&empty_mask);
        if (sigprocmask(SIG_SETMASK, &empty_mask, NULL) == -1) {
            perror("sigprocmask");
            exit(EXIT_FAILURE);
        }
    }

    while (!got_sigint) {
        /* Loop until SIGINT caught */
        continue;
    }

    for (n = 1; n < NSIG; ++n) {
        if (sig_cnt[n] != 0) {
            printf("%s: signal %d caught %d times%s\n", argv[0], n,
                    sig_cnt[n], (sig_cnt[n] == 1) ? " " : "s");
        }
    }

    exit(EXIT_SUCCESS);
}
