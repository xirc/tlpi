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


/* thread_incr_psem.c

   Use a POSIX unnamed semaphore to synchronize access by two threads to
   a global variable.

   See also thread_incr.c and thread_incr_mutex.c.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>


static int glob = 0;
static sem_t sem;


/* Loop 'arg' times incrementing 'glob' */
static void *
thread_func(void *arg)
{
    int loops = *((int*) arg);
    int loc, j;

    for (j = 0; j < loops; ++j) {
        if (sem_wait(&sem) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }

        loc = glob;
        loc++;
        glob = loc;

        if (sem_post(&sem) == -1) {
            perror("sem_post");
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

    loops = (argc > 1) ? strtol(argv[1], NULL, 0) : 10000000;
    if (loops <= 0) {
        fprintf(stderr, "num-loops %s > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Initialize a semaphore with the value 1 */
    if (sem_init(&sem, 0, 1) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    /* Create two threads that increment 'glob' */
    s = pthread_create(&t1, NULL, thread_func, &loops);
    if (s != 0) {
        errno = s;
        perror("pthread_create 1");
        exit(EXIT_FAILURE);
    }
    s = pthread_create(&t2, NULL, thread_func, &loops);
    if (s != 0) {
        errno = s;
        perror("pthread_create 2");
        exit(EXIT_FAILURE);
    }

    /* Wait for thread to terminate */
    s = pthread_join(t1, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_join 1");
        exit(EXIT_FAILURE);
    }
    s = pthread_join(t2, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_join 2");
        exit(EXIT_FAILURE);
    }

    printf("glob = %d\n", glob);

    exit(EXIT_SUCCESS);
}
