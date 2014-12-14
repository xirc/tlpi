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


/* fifo_seqnum_client.c

   A simple client that uses a well-known FIFO to request a (trivial)
   "sequence number service". This client creates its own FIFO (using a
   convention agreed upon by client and server) which is used to receive a reply
   from the server. The client then sends a request to the server consisting of
   its PID and the length of the sequence it wishes to be allocated. The client
   then reads the server's response and displays it on stdout.

   See fifo_seqnum.h for the format of request and response messages.

   The server is in fifo_seqnum_server.c.
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "fifo_seqnum.h"


static char client_fifo[CLIENT_FIFO_NAME_LEN];


/* Invoke on exit to delete client FIFO */
static void
remove_fifo(void)
{
    unlink(client_fifo);
}


int
main(int argc, char *argv[])
{
    int server_fd, client_fd;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [seq-len...]\n", argv[0]);
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

    /* Construct request message, open server FIFO, and send request */
    req.pid = getpid();
    req.seq_len = (argc > 1) ? atoi(argv[1]) : 1;
    if (req.seq_len <= 0) {
        fprintf(stderr, "seq-len '%s' > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

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

    /* Open our FIFO, read and display response */
    client_fd = open(client_fifo, O_RDONLY);
    if (client_fd == -1) {
        fprintf(stderr, "open %s: %s\n", client_fifo, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (read(client_fd, &resp, sizeof(struct response))
            != sizeof(struct response))
    {
        perror("Cannot read response from server");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", resp.seq_num);

    exit(EXIT_SUCCESS);
}
