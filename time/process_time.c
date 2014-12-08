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


/* process_time.c

   Demonstrate usage of clock(3) and times(2) to retrieve process virtual times.

   Usage: process_time [num-calls]

   Make 'num-calls' calls to getppid(), and then display process times.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <time.h>


/* Display 'msg' and process times */
static void
display_process_times(const char *msg)
{
    struct tms t;
    clock_t clockTime;
    static long clockTicks = 0;

    if (msg != NULL) {
        printf("%s", msg);
    }

    if (clockTicks == 0) {      /* Fetch clock ticks on first call */
        clockTicks = sysconf(_SC_CLK_TCK);
        if (clockTicks == -1) {
            perror("sysconf");
            exit(EXIT_FAILURE);
        }
    }

    clockTime = clock();
    if (clockTime == -1) {
        perror("clock");
        exit(EXIT_FAILURE);
    }

    printf("        clock() returns: %ld clocks-per-sec (%.2f secs)\n",
            (long) clockTime, (double) clockTime / CLOCKS_PER_SEC);

    if (times(&t) == -1) {
        perror("times");
        exit(EXIT_FAILURE);
    }
    printf("        times() yields: user CPU=%.2f; system CPU: %.2f\n",
            (double) t.tms_utime / clockTicks,
            (double) t.tms_stime / clockTicks);
}


int
main(int argc, char *argv[])
{
    int num_calls, j;

    printf("CLOCKS_PER_SEC=%ld  sysconf(_SC_CLK_TCK)=%ld\n\n",
            (long) CLOCKS_PER_SEC, sysconf(_SC_CLK_TCK));

    display_process_times("At program start:\n");

    /* Call getppid() a large number of times, so that
       some user and system CPU time are consumed */

    num_calls = (argc > 1) ? strtoul(argv[1], NULL, 0) : 100000000;
    for (j = 0; j < num_calls; j++) {
        (void) getppid();
    }

    display_process_times("After getppid() loop:\n");

    exit(EXIT_SUCCESS);
}
