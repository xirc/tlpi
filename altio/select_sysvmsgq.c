/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define BUF_SIZE       1024
#define MSGBUF_SIZE    10


static void
watch_msq(int msqid, int wepfd)
{
    struct msgbuf {
        long mtype;
        char mtext[MSGBUF_SIZE];
    } buf;
    int num_recv;

    while (1) {
        num_recv = msgrcv(msqid, &buf, MSGBUF_SIZE, 0, MSG_NOERROR);
        if (num_recv == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        if (write(wepfd, buf.mtext, num_recv) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
}


static int msqid;
static void
cleanup_handler(int sig __attribute__((unused)))
{
    (void) msgctl(msqid, IPC_RMID, NULL);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int msqpfd[2];
    fd_set readfds;
    int j, nfds, ready;
    char buf[1024];
    ssize_t num_read;
    struct sigaction sa;

    msqid = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    if (pipe2(msqpfd, O_NONBLOCK) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    switch(fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        watch_msq(msqid, msqpfd[1]);
        _exit(EXIT_SUCCESS);
    default:
        break;
    }


    /* Parent */

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = cleanup_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("select(2)\n");
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(msqpfd[0], &readfds);
        nfds = msqpfd[0] + 1;    /* In most cases, STDIN_FILENO = 0 (<msqfd[0]). */
        ready = select(nfds, &readfds, NULL, NULL, NULL);
        if (ready == -1) {
            perror("select");
            break;
        }
        for (j = 0; j < nfds; ++j) {
            if (!FD_ISSET(j, &readfds)) {
                continue;
            }
            printf(">>> FD=%d\n", j);
            while (1) {
                num_read = read(j, buf, BUF_SIZE);
                if (num_read == -1 && errno != EAGAIN) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                if (num_read <= 0) {
                    printf("\n<<<\n");
                    break;
                }
                printf("%.*s", (int)num_read, buf);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
