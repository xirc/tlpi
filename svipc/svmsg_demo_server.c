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


/* svmsg_demo_server.c

   Demonstration System V message queue-based server.
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define KEY_FILE "./svmsg_demo_server"
                /* Should be an existing file or
                 * one that this program creates */


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int msqid;
    key_t key;
    int const MQ_PERMS =
        S_IRUSR | S_IWUSR | S_IWGRP;    /* rw--w---- */

    /* Optional code here to check
     * if another server process is already running.
     * Generate the key for the message queue */
    key = ftok(KEY_FILE, 1);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    /* While msgget() fails, try creating the queue exclusively */
    while ((msqid = msgget(key, IPC_CREAT | IPC_EXCL | MQ_PERMS)) == -1) {
        if (errno == EEXIST) {
            /* MQ with the same key already exists -
             * remove it and try again */
            msqid = msgget(key, 0);
            if (msqid == -1) {
                perror("msgget() failed to retrieve old queue ID");
                exit(EXIT_FAILURE);
            }
            if (msgctl(msqid, IPC_RMID, NULL) == -1) {
                perror("msgctl() failed to delete old queue");
                exit(EXIT_FAILURE);
            }
            printf("Removed old message queue (id=%d)\n", msqid);
        } else {
            /* Some other error --> give up */
            perror("msgget() failed");
            exit(EXIT_FAILURE);
        }
    }

    /* Upon loop exit, we've successfully created the message queue, and
     * we can then carry on to do other work... */
    sleep(3);

    exit(EXIT_SUCCESS);
}
