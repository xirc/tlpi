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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "svmsg_file2.h"


static int client_id;

static void
remove_queue(void)
{
    if (msgctl(client_id, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
}


int
main(int argc, char *argv[])
{
    struct request_msg req;
    struct response_msg resp;
    int server_idfd, server_id, num_msgs;
    ssize_t msg_len, total_bytes;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s pathmae\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strlen(argv[1]) > sizeof(req.pathname) - 1) {
        fprintf(stderr, "pathmame too long (max: %ld bytes)\n",
                (long) sizeof(req.pathname) - 1);
        fprintf(stderr, "usage: %s pathname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Get server's queue identifier */
    server_idfd = open(SERVER_MSQID_FILE, O_RDONLY);
    if (server_idfd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (read(server_idfd, &server_id, sizeof(server_id)) != sizeof(server_id)) {
        fprintf(stderr, "cannot read server's key from file \"%s\"\n",
                SERVER_MSQID_FILE);
        exit(EXIT_FAILURE);
    }
    if (close(server_idfd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    /* Create queue for response */
    client_id = msgget(IPC_PRIVATE,
            S_IRUSR | S_IWUSR | S_IWGRP);   /* rw--w---- */
    if (client_id == -1) {
        perror("msgget - client message queue");
        exit(EXIT_FAILURE);
    }

    if (atexit(remove_queue) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    /* Send message asking for file named in argv[1] */
    req.mtype = 1;      /* Any type will do */
    req.client_id = client_id;
    strncpy(req.pathname, argv[1], sizeof(req.pathname) -1);
    req.pathname[sizeof(req.pathname) - 1] = '\0';
            /* Ensure string is terminated */

    if (msgsnd(server_id, &req, REQ_MSG_SIZE, 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    /* Get first response, which may be failure notification */
    msg_len = msgrcv(client_id, &resp, RESP_MSG_SIZE, 0, 0);
    if (msg_len == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data);      /* Display msg from server */
        if (msgctl(client_id, IPC_RMID, NULL) == -1) {
            perror("msgctl");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }

    /* File was opened successfully by server; process messages
     * (including the one already received) containing file data */
    total_bytes = msg_len;      /* Count first message */
    for (num_msgs = 1; resp.mtype == RESP_MT_DATA; ++num_msgs) {
        msg_len = msgrcv(client_id, &resp, RESP_MSG_SIZE, 0, 0);
        if (msg_len == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        total_bytes += msg_len;
    }

    printf("Received %ld bytes (%d messages)\n",
            (long) total_bytes, num_msgs);

    exit(EXIT_SUCCESS);
}
