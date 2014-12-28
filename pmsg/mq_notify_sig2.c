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


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>
#include <fcntl.h>


#define NOTIFY_SIG SIGUSR1


static void
handler(int sig __attribute__((unused)))
{
    /* Just interrupt sigsuspend() */
}


int
main(int argc, char *argv[])
{
    struct sigevent sev;
    mqd_t mqd;
    struct mq_attr attr;
    void *buffer;
    ssize_t num_read;
    sigset_t block_mask, empty_mask;
    struct sigaction sa;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s mq-name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    if (mq_getattr(mqd, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&block_mask);
    sigaddset(&block_mask, NOTIFY_SIG);
    if (sigprocmask(SIG_BLOCK, &block_mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(NOTIFY_SIG, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    if (mq_notify(mqd, &sev) == -1) {
        perror("mq_notify");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&empty_mask);
    while (1) {
        (void) sigsuspend(&empty_mask);     /* Wait for notification signal */
        /* mq_notify notifies only once */
#if 0
        if (mq_notify(mqd, &sev) == -1) {
            perror("mq_notify");
            exit(EXIT_FAILURE);
        }
#endif

        while ((num_read =
                    mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0)
        {
            printf("Read %ld bytes\n", (long) num_read);
        }
        if (errno != EAGAIN) {      /* Unexpected error */
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
