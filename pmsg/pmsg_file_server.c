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
#include <sys/wait.h>
#include <mqueue.h>

#include "pmsg_file.h"


static volatile sig_atomic_t is_running = 1;


/* SIGCHLD handler */
static void
grim_reaper(int sig __attribute__((unused)))
{
    int saved_errno;

    /* waitpid() might change 'errno' */
    saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
    errno = saved_errno;
}


/* SIGINT, SIGTERM handler */
static void
stop_handler(int sig __attribute__((unused)))
{
    is_running = 0;
}


/* Executed in child process: serve a single client */
static void
serve_request(struct request const *req)
{
    mqd_t client_qd;
    char client_pmsg[CLIENT_PMSG_NAME_LEN];
    int fd;
    struct response resp;
    ssize_t num_read;

    /* Open client PMSG (previously created by client) */
    /* Template for building client PMSG name */
    snprintf(client_pmsg, CLIENT_PMSG_NAME_LEN,
            CLIENT_PMSG_TEMPLATE, (long) req->client_id);
    client_qd = mq_open(client_pmsg, O_WRONLY);
    if (client_qd == -1) {
        fprintf(stderr, "Error open to POSIX MSGQ %s\n", client_pmsg);
        goto FAIL;
    }

    fd = open(req->pathname, O_RDONLY);
    if (fd == -1) {
        /* Open failed: send error test */
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        (void) mq_send(client_qd, (char*)&resp, sizeof(struct response), 0);
        fprintf(stderr, "Error open requested file\n");
        goto FAIL;
    }

    /* Transmit file contents in messages with type RESP_MT_DATA.
     * We don't diagnose read() and mq_send() errors
     * since we cannot notify client. */
    resp.mtype = RESP_MT_DATA;
    while ((num_read = read(fd, resp.data, RESP_MSG_SIZE)) > 0) {
        if (mq_send(client_qd, (char*)&resp,
                    sizeof(struct response), 0) == -1)
        {
            break;
        }
    }

    /* Send a message of type RESP_MT_END to signify end-of-file */
    resp.mtype = RESP_MT_END;
    if (mq_send(client_qd, (char*)&resp, sizeof(struct response), 0) == -1) {
        fprintf(stderr, "Error send to END-OF-FILE\n");
        goto FAIL;
    }

    if (mq_close(client_qd) == -1) {
        perror("mq_close");
    }
    return;

FAIL:
    (void) mq_close(client_qd);
    exit(EXIT_FAILURE);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{

    struct request req;
    pid_t pid;
    ssize_t msg_len;
    mqd_t server_qd;
    struct sigaction sa;
    struct mq_attr attr;

    /* Create server POSIX message queue */
    attr.mq_maxmsg = 8;
    attr.mq_msgsize = sizeof(struct request);
    server_qd = mq_open(SERVER_QUEUE_PATH, O_CREAT | O_EXCL,
            S_IRUSR | S_IWUSR, &attr);   /* rw------- */
    if (server_qd == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    /* Establish SIGCHLD handler to reap terminated children */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grim_reaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Establish SIGINT & SIGTERM handler to stop */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = stop_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Read requests, handle each in a separate child process */
    while (is_running) {
        msg_len = mq_receive(server_qd, (char*)&req,
                sizeof(struct request), NULL);
        if (msg_len == -1) {
            if (errno == EINTR) {   /* Interrupted by SIGCHLD handler ? */
                continue;           /* ... then restart mq_receive() */
            }
            perror("mq_receive");
            break;
        }

        /* Create child process */
        pid = fork();
        if (pid == -1) {
            perror("fork");
            break;
        }

        /* Child handles request */
        if (pid == 0) {
            serve_request(&req);
            _exit(EXIT_SUCCESS);
        }

        /* Parent loops to receive next client request */
    }

    /* If mq_receive() or fork() fails, remove server PMQ and exit */
    if (mq_unlink(SERVER_QUEUE_PATH) == -1) {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
    if (mq_close(server_qd) == -1) {
        perror("mq_close");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
