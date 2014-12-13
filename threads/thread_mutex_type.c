/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


static volatile int glob = 0;
static pthread_mutex_t mtx;


static void *
thread_func(void *arg)
{
    int s;

    s = pthread_mutex_lock(&mtx);
    if (s != 0) {
        fprintf(stderr, "pthread_mutex_lock (errno=%d, errmsg=%s)\n",
                s, strerror(s));
        exit(EXIT_FAILURE);
    }

    glob += *((int *) arg);

    s = pthread_mutex_unlock(&mtx);
    if (s!= 0) {
        fprintf(stderr, "pthread_mutex_unlock (errno=%d, errmsg=%s)\n",
                s, strerror(s));
        exit(EXIT_FAILURE);
    }

    return arg;
}


int
main(int argc,
     char *argv[] __attribute__((unused)))
{
    pthread_t t;
    pthread_mutexattr_t mtx_attr;
    int s;

    s = pthread_mutexattr_init(&mtx_attr);
    if (s != 0) {
        fprintf(stderr, "pthread_mutexattr_init (errno=%d errmsg=%s)\n",
                s, strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_mutexattr_settype(&mtx_attr, PTHREAD_MUTEX_ERRORCHECK);
    if (s != 0) {
        fprintf(stderr, "pthread_mutexattr_settype (errno=%d, errmsg=%s)\n",
                s, strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_mutex_init(&mtx, &mtx_attr);
    if (s != 0) {
        fprintf(stderr, "pthread_mutex_init (errno=%d, errmsg=%s)\n",
                s, strerror(s));
        exit(EXIT_FAILURE);
    }

    /* No longer needed */
    s = pthread_mutexattr_destroy(&mtx_attr);
    if (s != 0) {
        fprintf(stderr, "pthread_mutexattr_destroy (errno=%d, errmsg=%s)\n",
                s, strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_create(&t, NULL, thread_func, &argc);
    if (s != 0) {
        fprintf(stderr, "pthread_create (errno=%d, errmsg=%s)\n",
                s, strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_join(t, NULL);
    if (s != 0) {
        fprintf(stderr, "pthread_join (errno=%d, errmsg=%s)\n",
                s, strerror(s));
        exit(EXIT_FAILURE);
    }

    printf("glob = %d\n", glob);

    exit(EXIT_SUCCESS);
}
