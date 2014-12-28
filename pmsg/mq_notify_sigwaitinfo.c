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
#include <signal.h>
#include <mqueue.h>
#include <fcntl.h>


#define NOTIFY_SIG SIGUSR1


int
main(int argc, char *argv[])
{
    struct sigevent sev;
    mqd_t mqd;
    struct mq_attr attr;
    void *buffer;
    ssize_t num_read;
    sigset_t block_mask, wait_mask;
    siginfo_t siginfo;
    int opt_mqd;
    int opt_mqdval;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s mq-name [mqd]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argc == 3) {
        opt_mqd = 1;
        opt_mqdval = strtoul(argv[2], NULL, 0);
    }

    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    if (opt_mqd) {
        if (dup2(mqd, opt_mqdval) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        mqd = opt_mqdval;
    }
    printf("MQD: %ld\n", (long) mqd);

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

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    sev.sigev_value.sival_int = mqd;
    if (mq_notify(mqd, &sev) == -1) {
        perror("mq_notify");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, NOTIFY_SIG);
    while (1) {
        /* Wait for notification signal */
        if (sigwaitinfo(&wait_mask, &siginfo) == -1) {
            perror("sigwaitinfo");
            exit(EXIT_FAILURE);
        }
        printf("si_signo:   %d\n", siginfo.si_signo);
        printf("si_errno:   %d\n", siginfo.si_errno);
        printf("si_code:    %d\n", siginfo.si_code);
        /*printf("si_trapno:  %d\n", siginfo.si_trapno);*/
        printf("si_pid:     %d\n", siginfo.si_pid);
        printf("si_uid:     %d\n", siginfo.si_uid);
        printf("si_status:  %d\n", siginfo.si_status);
        printf("si_utime:   %ld\n", (long) siginfo.si_utime);
        printf("si_stime:   %ld\n", (long) siginfo.si_stime);
        printf("si_value:   %p (ptr)\n", siginfo.si_value.sival_ptr);
        printf("            %d (val)\n", siginfo.si_value.sival_int);
        printf("si_int:     %d\n", siginfo.si_int);
        printf("si_ptr:     %p\n", siginfo.si_ptr);
        printf("            %ld\n", (long) siginfo.si_ptr);
        printf("si_overrun: %d\n", siginfo.si_overrun);
        printf("si_timerid: %d\n", siginfo.si_timerid);
        printf("si_addr:    %p\n", siginfo.si_addr);
        printf("            %ld\n", (long) siginfo.si_addr);
        printf("si_band:    %ld\n", (long) siginfo.si_band);
        printf("si_fd:      %d\n", siginfo.si_fd);

        if (mq_notify(mqd, &sev) == -1) {
            perror("mq_notify");
            exit(EXIT_FAILURE);
        }

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
