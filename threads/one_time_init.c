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
#include <stdbool.h>
#include <string.h>
#include <pthread.h>


struct m_once_t {
    bool is_init;
    pthread_mutex_t mtx;
};


static int
one_time_init(struct m_once_t *control, void (*init)(void))
{
    int s;

    s = pthread_mutex_lock(&control->mtx);
    if (s != 0) {
        return s;
    }

    if (!control->is_init) {
        control->is_init = true;
        init();
    }

    s = pthread_mutex_unlock(&control->mtx);
    if (s != 0) {
        return s;
    }

    return 0;
}


static struct m_once_t m_once = { false, PTHREAD_MUTEX_INITIALIZER };
static pthread_once_t once = PTHREAD_ONCE_INIT;


static void
m_init(void)
{
    printf("initialize\n");
}


static void *
thread_func(void *arg)
{
    int use_pthread_once;

    use_pthread_once = *(int*)arg;

    if (use_pthread_once) {
        printf("call pthread_once\n");
        pthread_once(&once, m_init);
    } else {
        printf("call one_time_init\n");
        one_time_init(&m_once, m_init);
    }

    return NULL;
}


int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int use_pthread_once, s;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [use pthread_once]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    use_pthread_once = (argc > 1) ? 1 : 0;

    s = pthread_create(&t1, NULL, thread_func, &use_pthread_once);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    s = pthread_create(&t2, NULL, thread_func, &use_pthread_once);
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

    exit(EXIT_SUCCESS);
}
