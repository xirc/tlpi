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


/* thread_cleanup.c

   An example of thread cancellation using the POSIX threads API:
   demonstrate the use of pthread_cancel() and cleanup handlers.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static int glob = 0;


/* Free memory pointed to by 'arg' and unlock mutex */
static void
cleanup_handler(void *arg)
{
    int s;

    printf("cleanup: freeing block at %p\n", arg);
    free(arg);
    printf("cleanup: unlocking mutex\n");
    s = pthread_mutex_unlock(&mtx);
    if (s != 0) {
        errno = s;
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }
}


static void *
thread_func(void *arg __attribute__((unused)))
{
    int s;
    void *buf;      /* Buffer allocated by thread */

    buf = malloc(0x10000);  /* Not a cancellation point */
    printf("thread:  allocated memory at %p\n", buf);

    s = pthread_mutex_lock(&mtx);       /* Not a cancellation point */
    if (s != 0) {
        errno = s;
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }

    pthread_cleanup_push(cleanup_handler, buf);
    while (glob == 0) {
        s = pthread_cond_wait(&cond, &mtx);     /* A cancellation point */
        if (s != 0) {
            errno = s;
            perror("pthread_cond_wait");
            exit(EXIT_FAILURE);
        }
    }

    printf("thread:  condition wait loop completed\n");
    pthread_cleanup_pop(1);     /* Executes cleanup handler */
    return NULL;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pthread_t th;
    void *res;
    int s;

    s = pthread_create(&th, NULL, thread_func, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    sleep(2);           /* Give thread a chance to get started */

    if (argc == 1) {    /* Cancel thread */
        printf("main:    about to cancel thread\n");
        s = pthread_cancel(th);
        if (s != 0) {
            errno = s;
            perror("pthread_cancel");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("main:    about to signal condition variable\n");
        glob = 1;
        s = pthread_cond_signal(&cond);
        if (s != 0) {
            errno = s;
            perror("pthread_cond_signal");
            exit(EXIT_FAILURE);
        }
    }

    s = pthread_join(th, &res);
    if (s != 0) {
        errno = s;
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    if (res == PTHREAD_CANCELED) {
        printf("main:    thread was canceled\n");
    } else {
        printf("main:    thread terminated normally\n");
    }

    exit(EXIT_SUCCESS);
}
