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
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "semun.h"


static void
usage_exit(char *progname, int exit_code)
{
    fprintf(stderr, "usage: %s COUNT\n", progname);
    exit(exit_code);
}


int
main(int argc, char *argv[])
{
    int semid;
    unsigned long opt_count;
    unsigned long i;
    struct sembuf sop_wait, sop_post;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage_exit(argv[0], EXIT_SUCCESS);
    }
    if (argc < 2) {
        usage_exit(argv[0], EXIT_FAILURE);
    }
    opt_count = strtoul(argv[1], NULL, 0);

    /* Create semaphore */
    union semun arg;
    semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    /* Initialize semaphore */
    arg.val = 1;
    if (semctl(semid, /* semnum= */ 0, SETVAL, arg) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    /* Perform an operation on first semaphore */
    sop_wait.sem_num = 0;
    sop_wait.sem_op = -1;
    sop_wait.sem_flg = 0;
    sop_post.sem_num = 0;
    sop_post.sem_op = +1;
    sop_post.sem_flg = 0;

    for (i = 0; i < opt_count; ++i) {
        if (semop(semid, &sop_wait, 1) == -1) {
            perror("semop");
            exit(EXIT_FAILURE);
        }
        if (semop(semid, &sop_post, 1) == -1) {
            perror("semop");
            exit(EXIT_FAILURE);
        }
    }

    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl - RMID");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
