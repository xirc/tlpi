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


/* thread_cancel.c

   Demonstrate the use of pthread_cancel() to cancel a POSIX thread.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


static void *
thread_func(void *arg __attribute__((unused)))
{
    int j;

    printf("New thread started\n");         /* May be a cancellation point */
    for (j = 1; 1; j++) {
        printf("Loop %d\n", j);             /* May be a cancellation point */
        sleep(1);                           /* A cancellation point */
    }

    /* NOT REACHED */
    return NULL;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pthread_t th;
    int s;
    void *res;

    s = pthread_create(&th, NULL, thread_func, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    sleep(3);       /* Allow new thread to run a while */

    s = pthread_cancel(th);
    if (s != 0) {
        errno = s;
        perror("pthread_cancel");
        exit(EXIT_FAILURE);
    }

    s = pthread_join(th, &res);
    if (s != 0) {
        errno = s;
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    if (res == PTHREAD_CANCELED) {
        printf("Thread was canceled\n");
    } else {
        printf("Thread was not canceled (should not happen!)\n");
    }

    exit(EXIT_SUCCESS);
}
