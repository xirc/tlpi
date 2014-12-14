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


/* svmsg_file_server.c

   A file server that uses System V message queues to handle client requests
   (see svmsg_file_client.c). The client sends an initial request containing
   the name of the desired file, and the identifier of the message queue to be
   used to send the file contents back to the child. The server attempts to
   open the desired file. If the file cannot be opened, a failure response is
   sent to the client, otherwise the contents of the requested file are sent
   in a series of messages.

   This application makes use of multiple message queues. The server maintains
   a queue (with a well-known key) dedicated to incoming client requests. Each
   client creates its own private queue, which is used to pass response
   messages from the server back to the client.

   This program operates as a concurrent server, forking a new child process to
   handle each client request while the parent waits for further client requests.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "svmsg_file.h"


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


/* Executed in child process: serve a single client */
static void
serve_request(struct request_msg const *req)
{
    int fd;
    ssize_t num_read;
    struct response_msg resp;

    fd = open(req->pathname, O_RDONLY);
    if (fd == -1) {
        /* Open failed: send error test */
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        msgsnd(req->client_id, &resp, strlen(resp.data) + 1, 0);
        exit(EXIT_FAILURE); /* and terminate */
    }

    /* Transmit file contents in messages with type RESP_MT_DATA.
     * We don't diagnose read() and msgsnd() errors
     * since we cannot notify client. */
    resp.mtype = RESP_MT_DATA;
    while ((num_read = read(fd, resp.data, RESP_MSG_SIZE)) > 0) {
        if (msgsnd(req->client_id, &resp, num_read, 0) == -1) {
            break;
        }
    }

    /* Send a message of type RESP_MT_END to signify end-of-file */
    resp.mtype = RESP_MT_END;
    msgsnd(req->client_id, &resp, 0, 0);    /* Zero-length mtext */
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{

    struct request_msg req;
    pid_t pid;
    ssize_t msg_len;
    int server_id;
    struct sigaction sa;

    /* Create server message queue */
    server_id = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL |
            S_IRUSR | S_IWUSR | S_IWGRP);   /* rw--w---- */
    if (server_id == -1) {
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

    /* Read requests, handle each in a separate child process */
    while (1) {
        msg_len = msgrcv(server_id, &req, REQ_MSG_SIZE, 0, 0);
        if (msg_len == -1) {
            if (errno == EINTR) {   /* Interrupted by SIGCHLD handler ? */
                continue;           /* ... then restart msgrcv() */
            }
            perror("msgrcv");
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

    /* If msgrcv() or fork() fails, remove server MQ and exit */
    if (msgctl(server_id, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
