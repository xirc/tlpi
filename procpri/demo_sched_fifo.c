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


/* demo_sched_fifo.c

   This program demonstrates the use of realtime scheduling policies. It creates
   two processes, each running under the SCHED_FIFO scheduling policy. Each
   process executes a function that prints a message every quarter of a second
   of CPU time. After each second of consumed CPU time, the function and calls
   sched_yield() to yield the CPU to the other process. Once a process has
   consumed 3 seconds of CPU time, the function terminates.

   This program must be run as superuser, or (on Linux 2.6.12 and later)
   with a suitable RLIMIT_RTPRIO resource limit.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/times.h>


#define CSEC_STEP 25            /* CPU centiseconds between messages */
static void
use_CPU(char *msg)
{
    struct tms tms;
    int cpu_centisecs, prev_step, prev_sec;

    prev_step = 0;
    prev_sec = 0;
    while (1) {
        if (times(&tms) == -1) {
            perror("times");
            exit(EXIT_FAILURE);
        }
        cpu_centisecs =
            (tms.tms_utime + tms.tms_stime) *
            100 / sysconf(_SC_CLK_TCK);

        if (cpu_centisecs >= prev_step + CSEC_STEP) {
            prev_step += CSEC_STEP;
            printf("%s (PID %ld) cpu=%0.2f\n",
                    msg, (long) getpid(),
                    cpu_centisecs / 100.0);
        }

        if (cpu_centisecs > 300) {
            /* Terminate after 3 seconds */
            break;
        }

        if (cpu_centisecs >= prev_sec + 100) {
            /* Yield once/second */
            prev_sec = cpu_centisecs;
            sched_yield();
        }
    }
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct rlimit rlim;
    struct sched_param sp;
    cpu_set_t set;

    /* Disable buffering of stdout */
    setbuf(stdout, NULL);

    /* Confine all processes to a single CPU, so that the processes
       won't run in parallel on multi-CPU systems. */
    CPU_ZERO(&set);
    CPU_SET(0, &set);

    if (sched_setaffinity(getpid(), sizeof(set), &set) == -1) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }

    /* Establish a CPU time limit. This demonstrates how we can
       ensure that a runaway realtime process is terminated if we
       make a programming error. The resource limit is inherited
       by the child created using fork().

       An alternative technique would be to make an alarm() call in each
       process (since interval timers are not inherited across fork()). */

    rlim.rlim_cur = rlim.rlim_max = 50;
    if (setrlimit(RLIMIT_CPU, &rlim) == -1) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    /* Run the two processes in the lowest SCHED_FIFO priority */

    sp.sched_priority = sched_get_priority_min(SCHED_FIFO);
    if (sp.sched_priority == -1) {
        perror("sched_get_priority_min");
        exit(EXIT_FAILURE);
    }

    if (sched_setscheduler(0, SCHED_FIFO, &sp) == -1) {
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        use_CPU("child ");
        exit(EXIT_SUCCESS);

    default:
        use_CPU("parent");
        exit(EXIT_SUCCESS);
    }
}
