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


/* thread_incr.c

   This program employs two POSIX threads that increment the same global
   variable, without using any synchronization method. As a consequence,
   updates are sometimes lost.

   See also thread_incr_mutex.c.
*/


/* This program is wrong */
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


/* "volatile" prevents compiler optimizations
 * of arithmetic operations on 'glob' */
static volatile int glob = 0;


/* Loop 'arg' times incrementing 'glob' */
static void *
thread_func(void *arg)
{
    int loc, j, loops;

    loops = *((int *) arg);

    for (j = 0; j < loops; ++j) {
        loc = glob;
        loc++;
        glob = loc;
    }

    return NULL;
}


int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int loops, s;

    loops = (argc > 1) ? atoi(argv[1]) : 10000000;
    if (loops <= 0) {
        fprintf(stderr, "num-loops %s > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

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
