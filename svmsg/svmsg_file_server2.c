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


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>

#include "svmsg_file2.h"


#define TIMEOUT 3 /* secs */


static volatile sig_atomic_t server_id;


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


/* SIGINT and SIGTERM handler */
static void
cleanup_handler(int sig __attribute__((unused)))
{
    if (msgctl(server_id, IPC_RMID, NULL) == -1) {
        syslog(LOG_ERR, "msgctl: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (unlink(SERVER_MSQID_FILE) == -1) {
        syslog(LOG_ERR, "unlink %s: %s", SERVER_MSQID_FILE, strerror(errno));
        exit(EXIT_FAILURE);
    }

    (void) signal(sig, SIG_DFL);
    (void) raise(sig);
}

static void
nop_handler(int sig __attribute__((unused)))
{
    /* do nothing */
}

static int
timed_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg, time_t timeout_s)
{
    int retc;
    int saved_errno;
    struct sigaction sa;
    struct sigaction old_sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = nop_handler;
    if (sigaction(SIGALRM, &sa, &old_sa) == -1) {
        return -1;
    }

    alarm(timeout_s);
    retc = msgsnd(msqid, msgp, msgsz, msgflg);
    saved_errno = errno;
    alarm(0);

    if (sigaction(SIGALRM, &old_sa, NULL) == -1) {
        return -1;
    }

    errno = saved_errno;
    return retc;
}

/* Executed in child process: serve a single client */
/* For simplicity we don't handle EINTR */
static void
serve_request(struct request_msg const *req)
{
    int fd;
    ssize_t num_read;
    struct response_msg resp;

    /* Open serve file */
    fd = open(req->pathname, O_RDONLY);
    if (fd == -1) {
        /* Open failed: send error test */
        syslog(LOG_NOTICE, "Failed to open file '%s'", req->pathname);

        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        if (msgsnd(req->client_id, &resp, strlen(resp.data) + 1, 0) == -1) {
            syslog(LOG_NOTICE, "Failed to send message(RESP_MT_FAILURE) to client %d",
                    req->client_id);
            exit(EXIT_FAILURE);
        }

        syslog(LOG_NOTICE, "Send message(RESP_MT_FAILURE) to client");
        exit(EXIT_FAILURE);
    }

    /* Transmit file contents in messages with type RESP_MT_DATA.
     * We don't diagnose read() and msgsnd() errors
     * since we cannot notify client. */
    resp.mtype = RESP_MT_DATA;
    while ((num_read = read(fd, resp.data, RESP_MSG_SIZE)) > 0) {
        if (timed_msgsnd(req->client_id, &resp, num_read, 0, TIMEOUT) == -1) {
            if (errno == EINTR) {
                syslog(LOG_ERR, "No response from client %d", req->client_id);
                if (msgctl(req->client_id, IPC_RMID, NULL) == -1) {
                    syslog(LOG_ERR, "Cannot remove client message queue");
                }
            } else {
                syslog(LOG_ERR, "Failed to send message(RESP_MT_DATA) to client %d",
                        req->client_id);
            }
            exit(EXIT_FAILURE);
        }
    }
    if (num_read == -1) {
        syslog(LOG_ERR, "Failed to read file '%s' (errno=%d)\n",
                req->pathname, errno);
        exit(EXIT_FAILURE);
    }

    /* Send a message of type RESP_MT_END to signify end-of-file */
    resp.mtype = RESP_MT_END;
    if (msgsnd(req->client_id, &resp, 0, 0)) {    /* Zero-length mtext */
        syslog(LOG_ERR, "Failed to send message(RESP_MT_END) to client %d",
                req->client_id);
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        syslog(LOG_ERR, "Failed to close file '%s'", req->pathname);
        exit(EXIT_FAILURE);
    }

    return;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct request_msg req;
    pid_t pid;
    ssize_t msg_len;
    int server_idfd;
    struct sigaction sa;

    /* becomes daemon */
    if (daemon(0, 0) == -1) {
        perror("daemon");
        exit(EXIT_FAILURE);
    }

    /* Use syslog */
    openlog(NULL, LOG_CONS, LOG_USER);

    /* Create server message queue */
    server_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL |
            S_IRUSR | S_IWUSR | S_IWGRP);   /* rw--w---- */
    if (server_id == -1) {
        syslog(LOG_ERR, "msgget IPC_PRIVATE: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Establish SIGCHLD handler to reap terminated children */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grim_reaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        syslog(LOG_ERR, "sigaction - SIGCHLD: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    /* Establish SIGINT & SIGTERM handler to cleanup */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = cleanup_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        syslog(LOG_ERR, "sigaction - SIGINT: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        syslog(LOG_ERR, "sigaction - SIGTERM: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Well-known file notifies id of sever message queue */
    server_idfd = open(SERVER_MSQID_FILE,
                O_RDWR | O_CREAT | O_EXCL,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (server_idfd == -1) {
        syslog(LOG_ERR, "open %s: %s", SERVER_MSQID_FILE, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (write(server_idfd, (void*)&server_id, sizeof(server_id)) !=
            sizeof(server_id))
    {
        syslog(LOG_ERR, "cannot write key to file \"%s\"\n", SERVER_MSQID_FILE);
        exit(EXIT_FAILURE);
    }
    if (close(server_idfd) == -1) {
        syslog(LOG_ERR, "close %s: %s", SERVER_MSQID_FILE, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Read requests, handle each in a separate child process */
    while (1) {
        msg_len = msgrcv(server_id, &req, REQ_MSG_SIZE, 0, 0);
        if (msg_len == -1) {
            if (errno == EINTR) {   /* Interrupted by SIGCHLD handler ? */
                continue;           /* ... then restart msgrcv() */
            }
            syslog(LOG_ERR, "msgrcv: %s", strerror(errno));
            break;
        }

        /* Create child process */
        pid = fork();
        if (pid == -1) {
            syslog(LOG_ERR, "fork: %s", strerror(errno));
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
        syslog(LOG_ERR, "msgctl: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    /* And delete server key file */
    if (unlink(SERVER_MSQID_FILE) == -1) {
        syslog(LOG_ERR, "unlink %s: %s", SERVER_MSQID_FILE, strerror(errno));
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
