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
#include <pthread.h>


/*
 * This program is wrong
 * このプログラムは、新しく作成したthreadがメインスレッド(main)の
 * ローカル変数を参照する。メインスレッド終了後、新しいthreadが
 * その変数を参照した場合の動作は不定となる。
 */


struct something {
    int this;
    void* that;
    char it;
};


static void *
thread_func(void *arg)
{
    struct something *pbuf = (struct something *) arg;

    /* Do some work with structure pointed to by 'pbuf' */
    sleep(1);
    printf("address(pbuf): %p\n", (void*) pbuf);
    pbuf->this = 0;
    pbuf->that = NULL;
    pbuf->it = '\0';

    return (void*) 0;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pthread_t thread;
    int s;
    struct something buf;

    s = pthread_create(&thread, NULL, thread_func, (void*) &buf);
    if (s != 0) {
        errno = 0;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    pthread_exit(EXIT_SUCCESS);
}
