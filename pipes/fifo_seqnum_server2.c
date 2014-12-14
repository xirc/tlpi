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
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

#include "fifo_seqnum.h"


#define SERVER_SEQNUM_FILE "./fifo_seqnum_server.seqnum"


static volatile sig_atomic_t is_running = 1;


static void
cleanup_handler(int sig __attribute__((unused)))
{
    is_running = 0;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int server_fd, client_fd, seqnum_fd;
    char client_fifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int num_read;
    int seq_num;        /* This is our "service" */
    struct sigaction sa;

    /* Create well-known FIFO, and open it for reading */
    umask(0);   /* So we get the permissions we want */
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 &&
            errno != EEXIST)
    {
        perror("mkfifo " SERVER_FIFO);
        exit(EXIT_FAILURE);
    }
    server_fd = open(SERVER_FIFO, O_RDONLY);
    if (server_fd == -1) {
        perror("open " SERVER_FIFO);
        exit(EXIT_FAILURE);
    }

    /* Ignore SIGPIPE */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
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

    /* Initialize SEQNUM */
    seqnum_fd = open(SERVER_SEQNUM_FILE,
            O_RDWR | O_EXCL | O_SYNC,
            S_IRUSR | S_IWUSR | S_IRGRP /* rw-r----- */);
    if (seqnum_fd == -1 && errno != ENOENT) {
        exit(EXIT_FAILURE);
    }
    if (seqnum_fd == -1) {
        /* SERVER_SEQNUM_FILE does not exist */
        seqnum_fd = open(SERVER_SEQNUM_FILE,
                O_RDWR | O_CREAT | O_EXCL | O_SYNC,
                S_IRUSR | S_IWUSR | S_IRGRP /* rw-r----- */);
        if (seqnum_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        seq_num = 0;
    } else {
        num_read = read(seqnum_fd, &seq_num, sizeof(seq_num));
        if (num_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (num_read != sizeof(seq_num)) {
            fprintf(stderr,
                "Cannot read config file '" SERVER_SEQNUM_FILE "'\n");
            fprintf(stderr,
                "Use 0 as seq_num\n");
            seq_num = 0;
        }
    }

    /* Read requests and send responses */
    while (is_running) {
        num_read = read(server_fd, &req, sizeof(struct request));
        if (num_read == 0) {
            /* EOF is detected */
            printf("EOF is detected\n");
            printf("Reopen " SERVER_FIFO "\n");
            close(server_fd);
            server_fd = open(SERVER_FIFO, O_RDONLY);
            if (server_fd == -1 && errno != EINTR) {
                perror("open " SERVER_FIFO);
                exit(EXIT_FAILURE);
            }
            continue;
        } else if (num_read == -1) {
            perror("read");
            continue;
        } else if (num_read != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;       /* Either partial read or error */
        }

        /* Open client FIFO (previously created by client) */
        snprintf(client_fifo, CLIENT_FIFO_NAME_LEN,
                CLIENT_FIFO_TEMPLATE, (long) req.pid);
        client_fd = open(client_fifo, O_WRONLY);
        if (client_fd == -1) { /* Open failed, give up on client */
            fprintf(stderr, "open %s: %s\n", client_fifo, strerror(errno));
            continue;
        }

        /* Save seq_num */
        seq_num += req.seq_len;
        if (lseek(seqnum_fd, 0, SEEK_SET) == -1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }
        if (write(seqnum_fd, &seq_num, sizeof(seq_num)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        /* Send response and close FIFO */
        resp.seq_num = seq_num;
        if (write(client_fd, &resp, sizeof(struct response)) !=
                sizeof(struct response))
        {
            fprintf(stderr, "Error writing to FIFO %s\n", client_fifo);
        }
        if (close(client_fd) == -1) {
            perror("close");
        }
    }

    unlink(SERVER_FIFO);
    exit(EXIT_SUCCESS);
}
