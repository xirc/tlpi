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


/* ptmr_sigev_thread.c

   This program demonstrates the use of threads as the notification mechanism
   for expirations of a POSIX timer. Each of the program's command-line
   arguments specifies the initial value and interval for a POSIX timer. The
   format of these arguments is defined by the function itimerspecFromStr().

   The program creates and arms one timer for each command-line argument.
   The timer notification method is specified as SIGEV_THREAD, causing the
   timer notifications to be delivered via a thread that invokes threadFunc()
   as its start function. The threadFunc() function displays information
   about the timer expiration, increments a global counter of timer expirations,
   and signals a condition variable to indicate that the counter has changed.
   In the main thread, a loop waits on the condition variable, and each time
   the condition variable is signaled, the main thread prints the value of the
   global variable that counts timer expirations.

   Kernel support for Linux timers is provided since Linux 2.6. On older
   systems, an incomplete user-space implementation of POSIX timers
   was provided in glibc.
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
#include <pthread.h>

#include "itimerspec_from_string.h"


#define BUF_SIZE 1024


static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int expire_count = 0;


static char*
now(const char *format)
{
    static char buf[BUF_SIZE];
    time_t t;
    size_t s;
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);
    if (tm == NULL) {
        return NULL;
    }
    s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);
    return (s == 0) ? NULL : buf;
}


static void
thread_func(union sigval sv)
{
    timer_t *tidptr;
    int s;

    tidptr = sv.sival_ptr;

    printf("[%s] Thread notify\n", now("%T"));
    printf("    timer ID=%ld\n", (long) *tidptr);
    printf("    timer_getoverrun()=%d\n", timer_getoverrun(*tidptr));

    /* Increment counter variable shared with main thread and signal
     * condition variable to notify main thread of the change. */
    s = pthread_mutex_lock(&mtx);
    if (s != 0) {
        fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    expire_count += 1 + timer_getoverrun(*tidptr);

    s = pthread_mutex_unlock(&mtx);
    if (s != 0) {
        fprintf(stderr, "pthraed_mutex_unlock: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_cond_signal(&cond);
    if (s != 0) {
        fprintf(stderr, "pthread_cond_signal: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
}


int
main(int argc, char *argv[])
{
    struct sigevent sev;
    struct itimerspec ts;
    timer_t *tidlist;
    int s, j;

    if (argc < 2) {
        fprintf(stderr,
                "usage: %s secs[/nsecs][:int-secs[/int-nsecs]]...\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    tidlist = calloc(argc - 1, sizeof(timer_t));
    if (tidlist == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    sev.sigev_notify = SIGEV_THREAD;
        /* Notify via thread */
    sev.sigev_notify_function = thread_func;
        /* Thread start function */
    sev.sigev_notify_attributes = NULL;
        /* Could be pointer to pthread_attr_t structure */

    /* Create and start one timer for each command-line argument */
    for (j = 0; j < argc - 1; ++j) {
        itimerspec_from_string(argv[j + 1], &ts);

        sev.sigev_value.sival_ptr = &tidlist[j];
            /* Passed as argument to thread_func() */

        if (timer_create(CLOCK_REALTIME, &sev, &tidlist[j]) == -1) {
            perror("timer_create");
            exit(EXIT_FAILURE);
        }
        printf("Timer ID: %ld (%s)\n", (long) tidlist[j], argv[j + 1]);

        if (timer_settime(tidlist[j], 0, &ts, NULL) == -1) {
            perror("timer_settime");
            exit(EXIT_FAILURE);
        }
    }

    /* The main thread waits on a condition variable that is signaled
     * on each invocation of the thread notification function. We
     * print a message so that the user can see that this occurred. */
    s = pthread_mutex_lock(&mtx);
    if (s != 0) {
        fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    while (1) {
        s = pthread_cond_wait(&cond, &mtx);
        if (s != 0) {
            fprintf(stderr, "pthread_cond_wait: %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }
        printf("main(): expire_count = %d\n", expire_count);
    }
}
