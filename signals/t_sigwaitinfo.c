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


/* t_sigwaitinfo.c

   Demonstrate the use of sigwaitinfo() to synchronously wait for a signal.
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


int
main(int argc, char *argv[])
{
    int sig, delay_secs;
    siginfo_t si;
    sigset_t all_sigs;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [delay-secs]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%s: PID is %ld\n", argv[0], (long) getpid());

    /* Block all signals (except SIGKILL and SIGSTOP) */
    sigfillset(&all_sigs);
    if (sigprocmask(SIG_SETMASK, &all_sigs, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
    printf("%s: signals blocked\n", argv[0]);

    /* Delay so that signals can be sent to us */
    if (argc > 1) {
        delay_secs = atoi(argv[1]);
        if (delay_secs <= 0) {
            fprintf(stderr, "delay-secs %s > 0\n", argv[1]);
        }
        printf("%s: about to delay %s seconds\n", argv[0], argv[1]);
        sleep(delay_secs);
        printf("%s: finished delay\n", argv[0]);
    }

    while (1) {
        sig = sigwaitinfo(&all_sigs, &si);
        if (sig == -1) {
            perror("sigwaitinfo");
            exit(EXIT_FAILURE);
        }

        if (sig == SIGINT || sig == SIGTERM) {
            exit(EXIT_SUCCESS);
        }

        printf("got signal: %d (%s)\n", sig, strsignal(sig));
        printf("    si_signo=%d, si_code=%d (%s), si_value=%d\n",
                si.si_signo, si.si_code,
                (si.si_code == SI_USER) ? "SI_USER" :
                (si.si_code == SI_QUEUE) ? "SI_QUEUE" : "other",
                si.si_value.sival_int);
        printf("    si_pid=%ld, si_uid=%ld\n",
                (long) si.si_pid, (long) si.si_uid);
    }
}
