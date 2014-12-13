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


/* detached_attrib.c

   An example of the use of POSIX thread attributes (pthread_attr_t):
   creating a detached thread.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


static void *
thread_func(void *x)
{
    return x;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pthread_t thr;
    pthread_attr_t attr;
    int s;

    s = pthread_attr_init(&attr);       /* Assigns default values */
    if (s != 0) {
        errno = s;
        perror("pthread_attr_init");
        exit(EXIT_FAILURE);
    }

    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (s != 0) {
        errno = s;
        perror("pthread_attr_setdetachstate");
        exit(EXIT_FAILURE);
    }

    s = pthread_create(&thr, &attr, thread_func, (void *) 1);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    s = pthread_attr_destroy(&attr);    /* No longer needed */
    if (s != 0) {
        errno = s;
        perror("pthread_attr_destroy");
        pthread_exit((void*)EXIT_FAILURE);
    }

    s = pthread_join(thr, NULL);
    if (s != 0) {
        printf("pthread_join failed as expected (errno=%d, errmsg=%s)\n",
                s, strerror(s));
    }
    exit(EXIT_SUCCESS);
}
