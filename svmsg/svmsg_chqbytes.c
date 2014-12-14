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


/* svmsg_chqbytes.c

   Usage: svmsg_chqbytes msqid max-bytes

   Change the 'msg_qbytes' setting of the System V message queue identified
   by 'msqid'.
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int
main(int argc, char *argv[])
{
    struct msqid_ds ds;
    int msqid;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s msqid max-bytes\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Retrieve copy of associated data structure from kernel */
    msqid = atoi(argv[1]);
    if (msgctl(msqid, IPC_STAT, &ds) == -1) {
        perror("msgctl IPC_STAT");
        exit(EXIT_FAILURE);
    }
    printf("old: MSG_QBYTES = %ld\n", (long) ds.msg_qbytes);

    /* Update associated data structure in kernel */
    ds.msg_qbytes = atoi(argv[2]);
    if (msgctl(msqid, IPC_SET, &ds) == -1) {
        perror("msgctl IPC_SET");
        exit(EXIT_FAILURE);
    }
    printf("new: MSG_QBYTES = %ld\n", (long) ds.msg_qbytes);

    exit(EXIT_SUCCESS);
}
