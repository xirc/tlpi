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
#include <mqueue.h>

#include "pmsg_file.h"


static char client_pmsg[CLIENT_PMSG_NAME_LEN];


static void
remove_queue(void)
{
    if (mq_unlink(client_pmsg) == -1) {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
}


int
main(int argc, char *argv[])
{
    struct request req;
    struct response resp;
    int num_msgs;
    ssize_t msg_len, total_bytes;
    mqd_t server_qd, client_qd;
    struct mq_attr attr;

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

    /* Get server's queue identifier; create queue for response */
    server_qd = mq_open(SERVER_QUEUE_PATH, O_WRONLY);
    if (server_qd == -1) {
        perror("mq_open - server message queue");
        exit(EXIT_FAILURE);
    }
    attr.mq_maxmsg = 8;
    attr.mq_msgsize = sizeof(struct response);
    snprintf(client_pmsg, CLIENT_PMSG_NAME_LEN,
            CLIENT_PMSG_TEMPLATE, (long) getpid());
    client_qd = mq_open(client_pmsg, O_RDONLY | O_CREAT | O_EXCL,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &attr);  /* rw------- */
    if (client_qd == -1) {
        perror("mq_open - client message queue");
        exit(EXIT_FAILURE);
    }

    if (atexit(remove_queue) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    /* Send message asking for file named in argv[1] */
    req.client_id = getpid();
    strncpy(req.pathname, argv[1], sizeof(req.pathname) -1);
    req.pathname[sizeof(req.pathname) - 1] = '\0';
            /* Ensure string is terminated */

    if (mq_send(server_qd, (char*)&req,
                sizeof(struct request), 0) == -1)
    {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }

    /* Get first response, which may be failure notification */
    msg_len = mq_receive(client_qd, (char*)&resp,
            sizeof(struct response), NULL);
    if (msg_len == -1) {
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data);      /* Display msg from server */
        exit(EXIT_FAILURE);
    }

    /* File was opened successfully by server; process messages
     * (including the one already received) containing file data */
    total_bytes = msg_len;      /* Count first message */
    for (num_msgs = 1; resp.mtype == RESP_MT_DATA; ++num_msgs) {
        msg_len = mq_receive(client_qd, (char*)&resp,
                sizeof(struct response), NULL);
        if (msg_len == -1) {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }
        total_bytes += msg_len;
    }

    printf("Received %ld bytes (%d messages)\n",
            (long) total_bytes, num_msgs);

    exit(EXIT_SUCCESS);
}
