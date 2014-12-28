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


/* mq_notify_thread.c

   Demonstrate message notification via threads on a POSIX message queue.

   Linux supports POSIX message queues since kernel 2.6.6.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <mqueue.h>
#include <fcntl.h>


static void notify_setup(mqd_t *mqdp);


/* Thread notification function */
static void
thread_func(union sigval sv)
{
    ssize_t num_read;
    mqd_t *mqdp;
    void *buffer;
    struct mq_attr attr;

    mqdp = sv.sival_ptr;

    if (mq_getattr(*mqdp, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    notify_setup(mqdp);
    while ((num_read =
                mq_receive(*mqdp, buffer, attr.mq_msgsize, NULL)) >= 0)
    {
        printf("Read %ld bytes\n", (long) num_read);
    }
    if (errno != EAGAIN) {      /* Unexpected error */
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }

    free(buffer);
    pthread_exit(NULL);
}


static void
notify_setup(mqd_t *mqdp)
{
    struct sigevent sev;

    sev.sigev_notify = SIGEV_THREAD;            /* Notify via thread */
    sev.sigev_notify_function = thread_func;
    sev.sigev_notify_attributes = NULL;
            /* Could be pointer to pthread_attr structure */
    sev.sigev_value.sival_ptr = mqdp;      /* Argument to thread_func() */
    if (mq_notify(*mqdp, &sev) == -1) {
        perror("mq_notify");
        exit(EXIT_FAILURE);
    }
}


int
main(int argc, char *argv[])
{
    mqd_t mqd;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s mq-name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    notify_setup(&mqd);
    pause();
}
