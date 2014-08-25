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


/* prod_condvar.c

   A simple POSIX threads producer-consumer example using a condition variable.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>


static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int avail = 0;


static void *
thread_func(void *arg)
{
    int s, j, cnt;
    cnt = atoi((char *) arg);

    for (j = 0; j < cnt; j++) {
        sleep(1);

        /* Code to produce a unit omitted */

        s = pthread_mutex_lock(&mtx);
        if (s != 0) {
            errno = s;
            perror("pthread_mutex_lock");
            exit(EXIT_FAILURE);
        }

        avail++;        /* Let consumer know another unit is available */

        s = pthread_mutex_unlock(&mtx);
        if (s != 0) {
            errno = s;
            perror("pthread_mutex_unlock");
            exit(EXIT_FAILURE);
        }

        s = pthread_cond_signal(&cond);         /* Wake sleeping consumer */
        if (s != 0) {
            errno = s;
            perror("pthread_cond_signal");
            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}


int
main(int argc, char *argv[])
{
    pthread_t tid;
    int s, j;
    int tot_required;            /* Total number of units that all threads
                                   will produce */
    int num_consumed;            /* Total units so far consumed */
    time_t begin;

    begin = time(NULL);

    /* Create all threads */
    tot_required = 0;
    for (j = 1; j < argc; j++) {
        tot_required += atoi(argv[j]);

        s = pthread_create(&tid, NULL, thread_func, argv[j]);
        if (s != 0) {
            errno = s;
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    /* Use a polling loop to check for available units */
    num_consumed = 0;

    while (num_consumed < tot_required) {
        s = pthread_mutex_lock(&mtx);
        if (s != 0) {
            errno = s;
            perror("pthread_mutex_lock");
            exit(EXIT_FAILURE);
        }

        while (avail == 0) {            /* Wait for something to consume */
            s = pthread_cond_wait(&cond, &mtx);
            if (s != 0) {
                errno = s;
                perror("pthread_cond_wait");
                exit(EXIT_FAILURE);
            }
        }

        /* At this point, 'mtx' is locked... */

        while (avail > 0) {             /* Consume all available units */

            /* Do something with produced unit */

            num_consumed ++;
            avail--;
            printf("T=%ld: numConsumed=%d\n",
                    (long) (time(NULL) - begin),
                    num_consumed);
        }

        s = pthread_mutex_unlock(&mtx);
        if (s != 0) {
            errno = s;
            perror("pthread_mutex_unlock");
            exit(EXIT_FAILURE);
        }

        /* Perhaps do other work here that does not require mutex lock */
    }

    exit(EXIT_SUCCESS);
}
