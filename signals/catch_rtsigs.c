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


/* catch_rtsigs.c

   Usage: catch_rtsigs [block-time [handler-sleep-time]]
                              default=0   default=1

        block-time specifies an amount of time the program should
                pause after blocking all signals. This allows
                multiple signals to be sent to the process before
                it unblocks all the signals.

        handler-sleep-time specifies the amount of time the signal
                handler should sleep before returning. Using a
                nonzero value here allows us to slow things down
                so that we can see what happens when multiple
                signals are sent.

   After optionally blocking all (possible) signals and sleeping for
   'block-time' seconds, loop continuously using pause() to wait for
   any incoming signals.

   The program can be terminated by typing control-C (which generates
   SIGINT) or sending it SIGTERM.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>


static volatile int handler_sleep_time;
static volatile int sig_cnt = 0;     /* Number of signals received */
static volatile int all_done = 0;


static void
siginfo_handler(int sig, siginfo_t *si,
        void *ucontext __attribute__((unused)))
{
    /* Handler for signals established using SA_SIGINFO */
    /* UNSAFE: This handler uses non-async-signal-safe functions
     * (printf(); see section 21.1.2 */

    /* SIGINT or SIGTERM can be used to terminate program */
    if (sig == SIGINT || sig == SIGTERM) {
        all_done = 1;
        return;
    }

    sig_cnt++;
    printf("caught signal %d\n", sig);
    printf("    si_signo=%d, si_code=%d (%s), ",
            si->si_signo, si->si_code,
            (si->si_code == SI_USER) ? "SI_USER" :
            (si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
    printf("si_value=%d\n", si->si_value.sival_int);
    printf("    si_pid=%ld, si_uid=%ld\n",
            (long) si->si_pid, (long) si->si_uid);
    sleep(handler_sleep_time);
}


int
main(int argc, char *argv[])
{
    struct sigaction sa;
    int sig, block_time;
    sigset_t prev_mask, block_mask;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [block-time [handler-sleep-time]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%s: PID is %ld\n", argv[0], (long) getpid());

    handler_sleep_time = (argc > 2) ? atoi(argv[2]) : 1;
    if (handler_sleep_time < 0) {
        fprintf(stderr, "handler-sleep-time %s > 0\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    /* Establish handler for most signals, During execution of the handler,
     * mask all other signals to prevent handlers recursively interrupting
     * each other (which would make the output hard to read).
     */
    sa.sa_sigaction = siginfo_handler;
    sa.sa_flags = SA_SIGINFO;
    sigfillset(&sa.sa_mask);

    for (sig = 1; sig < NSIG; ++sig) {
        if (sig != SIGTSTP && sig != SIGQUIT) {
            sigaction(sig, &sa, NULL);
        }
    }

    /* Optionally block signals and sleep, allowing signals to be
     * sent to us before they are unblocked and handled
     */
    if (argc > 1) {
        sigfillset(&block_mask);
        sigdelset(&block_mask, SIGINT);
        sigdelset(&block_mask, SIGTERM);

        if (sigprocmask(SIG_SETMASK, &block_mask, &prev_mask) == -1) {
            perror("sigprocmask");
            exit(EXIT_FAILURE);
        }

        block_time = atoi(argv[1]);
        if (block_time < 0) {
            fprintf(stderr, "block-time %s > 0\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        printf("%s: signals blocked - sleeping %s seconds\n",
                argv[0], argv[1]);
        sleep(block_time);
        printf("%s: sleep complete\n", argv[0]);
        if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
            perror("sigprocmask");
            exit(EXIT_FAILURE);
        }
    }

    /* Wait for incoming signals */
    while (!all_done) {
        pause();
    }

    printf("Caught %d signals\n", sig_cnt);
    exit(EXIT_SUCCESS);
}
