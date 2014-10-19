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
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "svmsg_seqnum.h"


int
main(int argc, char *argv[])
{
    int msqid;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [seq-len...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Get message queue; message queue is created by server already. */
    msqid = msgget(MSQ_KEY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    /* Construct request message, and send request */
    req.mtype = SERVER_ID;
    req.pid = getpid();
    req.seq_len = (argc > 1) ? atoi(argv[1]) : 1;
    if (req.seq_len <= 0) {
        fprintf(stderr, "seq-len '%s' > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if (msgsnd(msqid, &req, REQ_MSG_SIZE, 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    /* Receive and display response */
    while (msgrcv(msqid, &resp, RESP_MSG_SIZE, getpid(), 0) == -1) {
        if (errno == EINTR) {
            continue;
        }
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("%s: %ld\n", argv[0], resp.seq_num);
    exit(EXIT_SUCCESS);
}
