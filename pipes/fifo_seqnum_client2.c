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
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include "fifo_seqnum.h"


static char client_fifo[CLIENT_FIFO_NAME_LEN];


static volatile sig_atomic_t is_running = 1;


/* Invoke on exit to delete client FIFO */
static void
remove_fifo(void)
{
    unlink(client_fifo);
}


/* Cleanup */
static void
cleanup_handler(int sig __attribute__((unused)))
{
    is_running = 0;
}


int
main(int argc, char *argv[])
{
    int server_fd, client_fd;
    struct request req;
    struct response resp;
    ssize_t num_read;
    struct sigaction sa;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s seq-len\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Create our FIFO (before sending request, to avoid a race) */
    umask(0);       /* So we get the permissions we want */
    snprintf(client_fifo, CLIENT_FIFO_NAME_LEN,
            CLIENT_FIFO_TEMPLATE, (long) getpid());
    if (mkfifo(client_fifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 &&
            errno != EEXIST)
    {
        fprintf(stderr, "mkfifo %s: %s\n", client_fifo, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (atexit(remove_fifo) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    /* Set cleanup handler */
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = cleanup_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Construct request message, open server FIFO, and send request */
    req.pid = getpid();
    req.seq_len = (argc > 1) ? atoi(argv[1]) : 1;
    if (req.seq_len <= 0) {
        fprintf(stderr, "seq-len '%s' > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    while (is_running) {
        /* Write the request */
        server_fd = open(SERVER_FIFO, O_WRONLY);
        if (server_fd == -1) {
            perror("open " SERVER_FIFO);
            exit(EXIT_FAILURE);
        }
        if (write(server_fd, &req, sizeof(struct request)) !=
                sizeof(struct request))
        {
            perror("cannot write to server");
            exit(EXIT_FAILURE);
        }
        if (close(server_fd) == -1) {
            perror("close " SERVER_FIFO);
            exit(EXIT_FAILURE);
        }

        /* Read the Response */
        /* Open our FIFO, read and display response */
        do {
            do {
                client_fd = open(client_fifo, O_RDONLY);
                if (client_fd == -1 && errno != EINTR) {
                    fprintf(stderr, "open %s: %s\n",
                            client_fifo, strerror(errno));
                    exit(EXIT_FAILURE);
                }
            } while (client_fd == -1);
            num_read = read(client_fd, &resp, sizeof(struct response));
            if (num_read == sizeof(struct response)) {
                printf("%d\n", resp.seq_num);
            } else if (num_read == 0) {
                /* Do nothing */
            } else if (num_read == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            } else {
                fprintf(stderr, "Cannot read whole response\n");
                exit(EXIT_FAILURE);
            }
            if (close(client_fd) == -1) {
                perror("close client fd");
                exit(EXIT_FAILURE);
            }
        } while (num_read == 0);
    }

    exit(EXIT_SUCCESS);
}
