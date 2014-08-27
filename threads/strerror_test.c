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


/* strerror_test.c

   A program to test whether the implementation of strerror() thread-safe.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>


static void *
thread_func(void *arg __attribute__((unused)))
{
    char *str;

    printf("Other thread about to call strerror()\n");
    str = strerror(EPERM);
    printf("Other thread: str (%p) = %s\n", str, str);

    return NULL;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pthread_t t;
    int s;
    char *str;

    str = strerror(EINVAL);
    printf("Main thread has called strerror()\n");

    s = pthread_create(&t, NULL, thread_func, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    s = pthread_join(t, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    printf("Main thread: str (%p) = %s\n", str, str);

    exit(EXIT_SUCCESS);
}
