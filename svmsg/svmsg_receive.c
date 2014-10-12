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


/* svmsg_receive.c

   Usage: svmsg_receive [-nex] [-t msg-type] msqid [max-bytes]

   Experiment with the msgrcv() system call to receive messages from a
   System V message queue.

   See also svmsg_send.c.
*/


#define _GNU_SOURCE
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


static void
usage(FILE *fp, char const *progname)
{
    fprintf(fp, "Usage: %s [options] msqid [max-bytes]\n", progname);
    fprintf(fp, "Permitted options are:\n");
    fprintf(fp, "    -e       Use MSG_NOERROR flag\n");
    fprintf(fp, "    -t type  Select message of given type\n");
    fprintf(fp, "    -n       Use IPC_NOWAIT flag\n");
#ifdef MSG_EXCEPT
    fprintf(fp, "    -x       Use MSG_EXCEPT flag\n");
#endif
}


int
main(int argc, char *argv[])
{
    int msqid, flags, type;
    ssize_t msg_len;
    size_t max_bytes;
    struct mbuf msg;            /* Message buffer for msgrcv() */
    int opt;                    /* Option character from getopt() */

    /* Parse command-line options */
    flags = 0;
    type = 0;
    while ((opt = getopt(argc, argv, "ent:x")) != -1) {
        switch (opt) {
        case 'e':       flags |= MSG_NOERROR;   break;
        case 'n':       flags |= IPC_NOWAIT;    break;
        case 't':       type = atoi(optarg);    break;
#ifdef MSG_EXCEPT
        case 'x':       flags |= MSG_EXCEPT;    break;
#endif
        default:
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (argc < optind + 1 || argc > optind + 2) {
        fprintf(stderr, "Wrong number of arguments\n");
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    msqid = atoi(argv[optind]);
    max_bytes = (argc > optind + 1) ?
        atoi(argv[optind + 1]) : MAX_MTEXT;

    /* Get message and display on stdout */
    msg_len = msgrcv(msqid, &msg, max_bytes, type, flags);
    if (msg_len == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("Received: type=%ld; length=%ld", msg.mtype, (long) msg_len);
    if (msg_len > 0) {
        printf("; body=%s", msg.mtext);
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}
