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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>

#include "pmsg_seqnum.h"


/* This is our service */
static volatile int sequence_number = 0;
static volatile sig_atomic_t is_running = 1;


static void
cleanup_handler(int sig __attribute__((unused)))
{
    is_running = 0;
}


static int
handle_request(struct request const *req)
{
    mqd_t client_qd;
    char client_pmsg[CLIENT_PMSG_NAME_LEN];
    struct response resp;

    /* Open client PMSG (previously created by client) */
    /* Template for building client PMSG name */
    snprintf(client_pmsg, CLIENT_PMSG_NAME_LEN,
            CLIENT_PMSG_TEMPLATE, (long) req->pid);
    client_qd = mq_open(client_pmsg, O_WRONLY);
    if (client_qd == -1) {
        fprintf(stderr, "Error open to POSIX MSGQ %s\n", client_pmsg);
        goto FAIL;
    }

    /* Send response and close PMSG */
    resp.seq_num = sequence_number;
    if (mq_send(client_qd, (char*)&resp,
                sizeof(struct response), 0) == -1)
    {
        fprintf(stderr, "Error writing to POSIX MSGQ %s\n", client_pmsg);
        goto FAIL;
    }
    sequence_number += req->seq_len;

    if (mq_close(client_qd) == -1) {
        perror("mq_close");
        goto FAIL;
    }
    return 0;

FAIL:
    (void) mq_close(client_qd);
    return -1;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    mqd_t server_qd;
    struct request req;
    struct mq_attr attr;
    struct sigaction sa;

    /* Create well-known server's POSIX message queue,
     * and open it for reading */
    attr.mq_maxmsg = 8;
    attr.mq_msgsize = sizeof(struct request);
    server_qd = mq_open(SERVER_PMSG, O_RDONLY | O_CREAT | O_EXCL,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &attr);
    if (server_qd == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    /* Set cleanup handler */
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = cleanup_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction - SIGINT");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction - SIGTERM");
        exit(EXIT_FAILURE);
    }

    /* Read requests and send responses */
    while (is_running) {
        if (mq_receive(server_qd, (char*)&req,
                    sizeof(struct request), NULL) != sizeof(struct request))
        {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;       /* Either partial read or error */
        }

        if (handle_request(&req) == -1) {
            fprintf(stderr, "Error at handling request\n");
        }
    }

    if (mq_unlink(SERVER_PMSG) == -1) {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
    if (mq_close(server_qd) == -1) {
        perror("mq_close");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
