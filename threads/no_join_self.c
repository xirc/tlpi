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


static int
pthread_join2(pthread_t th, void **thread_return)
{
    if (pthread_equal(th, pthread_self())) {
        return 0;
    }
    return pthread_join(th, thread_return);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int s;

    s = pthread_join2(pthread_self(), NULL);
    if (s != 0) {
        fprintf(stderr, "pthread_join (errno=%d, errmsg=%s)\n",
                s, strerror(s));
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
