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
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>

#include "pmsg_seqnum.h"


static char client_pmsg[CLIENT_PMSG_NAME_LEN];


/* Invoke on exit to delete client POSIX message queue */
static void
remove_pmsg(void)
{
    (void) mq_unlink(client_pmsg);
}


int
main(int argc, char *argv[])
{
    int server_qd, client_qd;
    struct request req;
    struct response resp;
    struct mq_attr attr;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [seq-len...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Create our POSIX message queue
     * (before sending request, to avoid a race) */
    snprintf(client_pmsg, CLIENT_PMSG_NAME_LEN,
            CLIENT_PMSG_TEMPLATE, (long) getpid());
    attr.mq_maxmsg = 8;
    attr.mq_msgsize = sizeof(struct response);
    client_qd = mq_open(client_pmsg, O_RDONLY | O_CREAT | O_EXCL,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &attr);
    if (client_qd == -1)
    {
        fprintf(stderr, "mq_open %s: %s\n", client_pmsg, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (atexit(remove_pmsg) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    /* Construct request message,
     * open server POSIX message queue, and send request */
    req.pid = getpid();
    req.seq_len = (argc > 1) ? atoi(argv[1]) : 1;
    if (req.seq_len <= 0) {
        fprintf(stderr, "seq-len '%s' > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    server_qd = mq_open(SERVER_PMSG, O_WRONLY);
    if (server_qd == -1) {
        perror("mq_open " SERVER_PMSG);
        exit(EXIT_FAILURE);
    }

    if (mq_send(server_qd, (char*) &req, sizeof(struct request), 0) == -1)
    {
        perror("cannot send to server");
        exit(EXIT_FAILURE);
    }

    if (mq_receive(client_qd, (char*) &resp, sizeof(struct response), NULL)
            != sizeof(struct response))
    {
        perror("Cannot receive response from server");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", resp.seq_num);

    exit(EXIT_SUCCESS);
}
