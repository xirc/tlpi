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


/* t_sigsuspend.c

   A short program to demonstrate why sigsuspend(&mask) is preferable to
   calling sigprocmask(SIG_SETMASK, &mask, NULL) + pause() separately.
   (By default this program uses sigsuspend(). To make it use pause(),
   compile using "cc -DUSE_PAUSE".)

   Usage: t_sigsuspend [sleep-time]

   Send the SIGINT signal to this program by typing control-C (^C).
   (Terminate the program using SIGQUIT, i.e., type control-\ (^\).)

   This program contains extra code that does not appear in the version shown
   in the book. By defining USE_PAUSE when compiling, we can replace the use of
   sigsuspend() by the nonatomic sigprocmask() + pause(). This allows us to
   show that doing the latter way will cause some signals to be lost.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include "signal_functions.h"


static volatile sig_atomic_t got_sigquit = 0;


static void
handler(int sig)
{
    /* UNSAFE (see section 21.1.2) */
    printf("Caught signal %d (%s)\n", sig, strsignal(sig));
    if (sig == SIGQUIT) {
        got_sigquit = 1;
    }
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int loop_num;
    time_t start_time;
    sigset_t orig_mask, block_mask;
    struct sigaction sa;

    print_sigmask(stdout, "Initial signal mask is:\n");

    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGINT);
    sigaddset(&block_mask, SIGQUIT);
    if (sigprocmask(SIG_BLOCK, &block_mask, &orig_mask) == -1) {
        perror("sigprocmask - SIG_BLOCK");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    for (loop_num = 1; !got_sigquit; loop_num++) {
        printf("=== LOOP %d\n", loop_num);

        /* Simulate a critical section by delaying a few seconds */
        print_sigmask(stdout,
                "Starting critical section, signal mask is:\n");
        for (start_time = time(NULL); time(NULL) < start_time + 4; ) {
            continue;
        }

        print_pending_sigs(stdout,
                "Before sigsuspend() - pending signals:\n");
        if (sigsuspend(&orig_mask) == -1 && errno != EINTR) {
            perror("sigsuspend");
            exit(EXIT_FAILURE);
        }
    }

    if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) == -1) {
        perror("sigprocmask - SIG_SETMASK");
        exit(EXIT_FAILURE);
    }

    print_sigmask(stdout, "=== Exited loop\nRestored signal mask to:\n");
    /* Do other processing... */
    exit(EXIT_SUCCESS);
}
