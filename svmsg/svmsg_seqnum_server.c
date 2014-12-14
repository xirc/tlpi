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
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

#include "svmsg_seqnum.h"


static volatile sig_atomic_t is_running = 1;


static void
grim_reaper(int sig __attribute__((unused)))
{
    int saved_errno;

    /* Wait all dead children */
    saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
    errno = saved_errno;
}


static void
cleanup_handler(int sig __attribute__((unused)))
{
    is_running = 0;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int msqid;
    struct request req;
    struct response resp;
    struct sigaction sa;
    ssize_t num_read;
    long seq_num;        /* This is our "service" */

    /* Create message queue */
    msqid = msgget(MSQ_KEY, IPC_CREAT | IPC_EXCL |
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    /* Set cleanup handler */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = cleanup_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction - SIGINT");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction - SIGTERM");
        exit(EXIT_FAILURE);
    }

    /* Establish SIGCHLD handler to reap terminated children */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grim_reaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction - SIGCHLD");
        exit(EXIT_FAILURE);
    }

    /* Setup service 'seq_num' */
    seq_num = 0;

    /* Read requests and send responses */
    while (is_running) {
        num_read = msgrcv(msqid, &req, REQ_MSG_SIZE, SERVER_ID, 0);
        if (num_read == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        if (req.pid == 0) {
            fprintf(stderr, "BAD REQUEST (pid=0)\n");
            continue;
        }
        resp.mtype = req.pid;
        resp.pid = req.pid;
        resp.seq_num = seq_num;
        while (is_running && msgsnd(msqid, &resp, RESP_MSG_SIZE, IPC_NOWAIT) == -1) {
            if (errno == EAGAIN) {
                /* Wait a second */
                sleep(1);
                continue;
            }
            if (errno == EINTR) {
                continue;
            }
        }
        printf("%s: serve %ld - %ld\n", argv[0], seq_num, seq_num + req.seq_len - 1);
        seq_num += req.seq_len;
    }

    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
