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


/* thread_multijoin.c

   This program creates one thread for each of its command-line arguments.
   Each thread sleeps for the number of seconds specified in the corresponding
   command-line argument, and then terminates. This sleep interval simulates
   "work" done by the thread.

   The program maintains a global array (pointed to by 'thread') recording
   information about all threads that have been created. The items of this
   array record the thread ID ('tid') and current state ('state', of type
   'enum tstate') of each thread.

   As each thread terminates, it sets its 'state' to TS_TERMINATED and
   signals the 'threadDied' condition variable. The main thread continuously
   waits on this condition variable, and is thus informed when any of the
   threads that it created has terminated. When 'numLive', which records
   the number of live threads, falls to 0, the main thread terminates.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


static pthread_cond_t thread_died = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;
    /* Protects all of the following global variables */

static int tot_threads = 0;     /* Total number of threads created */
static int num_live = 0;        /* Total number of threads still alive or
                                   terminated but not yet joined */
static int num_unjoined = 0;    /* Number of terminated threads that
                                   have not yet been joined */
enum tstate {                   /* Thread states */
    TS_ALIVE,                   /* Thread is alive */
    TS_TERMINATED,              /* Thread terminated, not yet joined */
    TS_JOINED                   /* Thread terminated, and joined */
};

static struct {                 /* Info about each thread */
    pthread_t tid;              /* ID of this thread */
    enum tstate state;          /* Thread state (TS_* constants above) */
    int sleep_time;             /* Number seconds to live before terminating */
} *thread;


/* Start function for thread */
static void *
thread_func(void *arg)
{
    int s, idx;

    idx = (int) arg;
    sleep(thread[idx].sleep_time);  /* Simulate doing some work */
    printf("Thread %d terminating\n", idx);

    s = pthread_mutex_lock(&thread_mutex);
    if (s != 0) {
        errno = s;
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }

    num_unjoined++;
    thread[idx].state = TS_TERMINATED;

    s = pthread_mutex_unlock(&thread_mutex);
    if (s != 0) {
        errno = s;
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }

    s = pthread_cond_signal(&thread_died);
    if (s != 0) {
        errno = s;
        perror("pthread_cond_signal");
        exit(EXIT_FAILURE);
    }

    return NULL;
}


int
main(int argc, char *argv[])
{
    int s, idx;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s nsecs...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    thread = calloc(argc - 1, sizeof(*thread));
    if (thread == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Create all threads */
    for (idx = 0; idx < argc - 1; ++idx) {
        thread[idx].sleep_time = atoi(argv[idx+1]);
        if (thread[idx].sleep_time < 0) {
            fprintf(stderr, "nsecs[%d] %s > 0\n", idx, argv[idx+1]);
            exit(EXIT_FAILURE);
        }
        thread[idx].state = TS_ALIVE;
        s = pthread_create(&thread[idx].tid, NULL, thread_func, (void*) idx);
        if (s != 0) {
            errno = s;
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    tot_threads = argc - 1;
    num_live = tot_threads;

    /* Join with terminated threads */
    while (num_live > 0) {
        s = pthread_mutex_lock(&thread_mutex);
        if (s != 0) {
            errno = s;
            perror("pthread_mutex_lock");
            exit(EXIT_FAILURE);
        }

        while (num_unjoined == 0) {
            s = pthread_cond_wait(&thread_died, &thread_mutex);
            if (s != 0) {
                errno = s;
                perror("pthread_cond_wait");
                exit(EXIT_FAILURE);
            }
        }

        for (idx = 0; idx < tot_threads; ++idx) {
            if (thread[idx].state == TS_TERMINATED) {
                s = pthread_join(thread[idx].tid, NULL);
                if (s != 0) {
                    errno = s;
                    perror("pthread_join");
                    exit(EXIT_FAILURE);
                }

                thread[idx].state = TS_JOINED;
                num_live--;
                num_unjoined--;

                printf("Repeated thread %d (num_live=%d)\n", idx, num_live);
            }
        }

        s = pthread_mutex_unlock(&thread_mutex);
        if (s != 0) {
            errno =s;
            perror("pthread_mutex_unlock");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
