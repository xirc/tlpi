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


/* thread_incr_mutex.c

   This program employs two POSIX threads that increment the same global
   variable, synchronizing their access using a mutex. As a consequence,
   updates are not lost. Compare with thread_incr.c.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


static int glob = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;


/* Loop 'arg' times incrementing 'glob' */
static void *
thread_func(void *arg)
{
    int loc, j, s, loops;

    loops = *((int *) arg);
    for (j = 0; j < loops; ++j) {
        s = pthread_mutex_lock(&mtx);
        if (s != 0) {
            errno = s;
            perror("pthread_mutex_lock");
            exit(EXIT_FAILURE);
        }

        loc = glob;
        loc++;
        glob = loc;

        s = pthread_mutex_unlock(&mtx);
        if (s != 0) {
            errno = s;
            perror("pthread_mutex_unlock");
            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}


int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int loops, s;

    loops = (argc > 1) ? atoi(argv[1]) : 10000000;

    s = pthread_create(&t1, NULL, thread_func, &loops);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    s = pthread_create(&t2, NULL, thread_func, &loops);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    s = pthread_join(t1, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    s = pthread_join(t2, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}
