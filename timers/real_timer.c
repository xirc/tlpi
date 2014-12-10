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


/* real_timer.c

   A demonstration of the use of (real-time) timers created using setitimer().

   Usage: real_timer [secs [usecs [int-secs [int-usecs]]]]
             Defaults: 2      0        0         0

   The command-line arguments are the second and microsecond settings for
   the timer's initial value and interval.

   The version of this code shown in the first print of the book contained
   an error in the main() function whereby 'maxSigs' was initialized before
   'itv', even though the initialization of the former variable depends on the
   initialization of the latter variable. See the erratum for page 983 to 984.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>


/* Set nonzero on receipt of SIGALRM */
static volatile sig_atomic_t got_alarm = 0;


/* Retrieve and display the real time, and (if 'include_timer' is TRUE)
 * the current value and interval for the ITIMER_REAL timer */
static void
display_times(const char *msg, int include_timer)
{
    struct itimerval itv;
    static struct timeval start;
    struct timeval curr;
    /* Number of calls to this function */
    static int call_num = 0;

    /* Initialize elapsed time meter */
    if (call_num == 0) {
        if (gettimeofday(&start, NULL) == -1) {
            perror("gettimeofday");
            exit(EXIT_FAILURE);
        }
    }

    /* Print header every 20 lines */
    if (call_num % 20 == 0) {
        printf("       Elapsed   Value Interval\n");
    }

    if (gettimeofday(&curr, NULL) == -1) {
        perror("gettimeofday");
        exit(EXIT_FAILURE);
    }
    printf("%-7s %6.2f", msg,
            curr.tv_sec - start.tv_sec +
            (curr.tv_usec - start.tv_usec) * 1e-6);
    if (include_timer) {
        if (getitimer(ITIMER_REAL, &itv) == -1) {
            perror("getitimer");
            exit(EXIT_FAILURE);
        }
        printf("  %6.2f  %6.2f",
                itv.it_value.tv_sec + itv.it_value.tv_usec * 1e-6,
                itv.it_interval.tv_sec + itv.it_interval.tv_usec * 1e-6);
    }
    printf("\n");

    call_num++;
}


static void
sigalrm_handler(int sig __attribute__((unused)))
{
    got_alarm = 1;
}


int
main(int argc, char *argv[])
{
    struct itimerval itv;
    clock_t prev_clock;
    /* Number of signals to catch before exiting */
    int max_sigs;
    /* Number of signals so far caught */
    int sig_count;
    struct sigaction sa;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr,
                "%s [secs [usecs [int-secs [int-usecs]]]]\n", argv[0]);
    }

    sig_count = 0;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigalrm_handler;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Set timer from the command-line arguments */
    itv.it_value.tv_sec = (argc > 1) ? atoi(argv[1]) : 2;
    itv.it_value.tv_usec = (argc > 2) ? atoi(argv[2]) : 0;
    itv.it_interval.tv_sec = (argc > 3) ? atoi(argv[3]) : 0;
    itv.it_interval.tv_usec = (argc > 4) ? atoi(argv[4]) : 0;

    /* Exit after 3 signals, or on first signal if interval is 0 */
    max_sigs = (itv.it_interval.tv_sec == 0 &&
                itv.it_interval.tv_usec == 0) ? 1 : 3;

    display_times("START:", 0);
    if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }

    prev_clock = clock();
    sig_count = 0;

    while (1) {
        /* Inner loop consumes at least 0.5 seconds CPU time */
        while (((clock() - prev_clock) * 10 / CLOCKS_PER_SEC) < 5) {
            /* Did we get a signal? */
            if (got_alarm) {
                got_alarm = 0;
                display_times("ALARM:", 1);

                sig_count++;
                if (sig_count >= max_sigs) {
                    printf("That's all folks\n");
                    exit(EXIT_SUCCESS);
                }
            }
        }
        prev_clock = clock();
        display_times("Main: ", 1);
    }
}
