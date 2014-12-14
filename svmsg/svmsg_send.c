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


/* svmsg_send.c

   Usage: svmsg_send [-n] msqid msg-type [msg-text]

   Experiment with the msgsnd() system call to send messages to a
   System V message queue.

   See also svmsg_receive.c.
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define MAX_MTEXT 1024
struct mbuf {
    long mtype;                 /* Message type */
    char mtext[MAX_MTEXT];      /* Message body */
};


/* Print usage description */
static void
usage(FILE *fp, char const *progname)
{
    fprintf(fp, "Usage: %s [-n] msqid msg-type [msg-text]\n", progname);
    fprintf(fp, "    -n       Usage IPC_NOWAIT flag\n");
}


int
main(int argc, char *argv[])
{
    int msqid, flags, msg_len;
    struct mbuf msg;                /* Message buffer for msgsnd() */
    int opt;                        /* Option character from getopt() */

    /* Parse command-line options and arguments */
    flags = 0;
    while ((opt = getopt(argc, argv, "n")) != -1) {
        switch (opt) {
        case 'n':
            flags |= IPC_NOWAIT;
            break;

        default:
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (argc < optind + 2 || argc > optind + 3) {
        fprintf(stderr, "Wrong number of arguments\n");
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    msqid = atoi(argv[optind]);
    msg.mtype = atoi(argv[optind + 1]);

    if (argc > optind + 2) {
            /* 'msg-text' was supplied */
        msg_len = strlen(argv[optind + 2]) + 1;
        if (msg_len > MAX_MTEXT) {
            fprintf(stderr,
                    "msg-text too long (max %d characters)\n", MAX_MTEXT);
            exit(EXIT_FAILURE);
        }
        memcpy(msg.mtext, argv[optind + 2], msg_len);
    } else {
            /* No 'msg-text' ==> zero-length msg */
        msg_len = 0;
    }

    /* Send message */
    if (msgsnd(msqid, &msg, msg_len, flags) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
