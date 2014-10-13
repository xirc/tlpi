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
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>


#define IPCMNI 32768


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int msqid;
    key_t key;
    struct msqid_ds ds;
    int const MQ_PERMS = S_IRUSR | S_IWUSR | S_IRGRP;   /* rw-r----- */

    key = ftok(__FILE__, 1);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    while ((msqid = msgget(key, IPC_CREAT | IPC_EXCL | MQ_PERMS)) == -1) {
        if (errno == EEXIST) {
            msqid = msgget(key, 0);
            if (msqid == -1) {
                perror("msgget");
                exit(EXIT_FAILURE);
            }
            if (msgctl(msqid, IPC_RMID, NULL) == -1) {
                perror("msgctl");
                exit(EXIT_FAILURE);
            }
        } else {
            perror("msgget()");
            exit(EXIT_FAILURE);
        }
        printf("Removed old message queue (id=%d)\n", msqid);
    }

    if (msgctl(msqid, IPC_STAT, &ds) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    printf("key:              %d\n", key);
    printf("id:               %d\n", msqid);
    printf("xxx_perm.__seq:   %d\n", ds.msg_perm.__seq);
    printf("SEQ_MULTIPLIER:   %d\n", IPCMNI);
    printf("base:             %d\n", ds.msg_perm.__seq * IPCMNI);
    printf("index:            %d\n", msqid % IPCMNI);

    exit(EXIT_SUCCESS);
}
