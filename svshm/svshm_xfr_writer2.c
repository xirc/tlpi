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

#include "semun.h"
#include "svshm_xfr.h"
#include "event_flag.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int semid, shmid, bytes, xfrs;
    struct shmseg *shmp;
    union semun dummy;

    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    if (init_event_flag(semid, WRITE_SEM) == -1) {
        perror("init_event_flag");
        exit(EXIT_FAILURE);
    }
    if (set_event_flag(semid, WRITE_SEM) == -1) {
        perror("set_event_flag");
        exit(EXIT_FAILURE);
    }
    if (init_event_flag(semid, READ_SEM) == -1) {
        perror("init_event_flag");
        exit(EXIT_FAILURE);
    }

    shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void*) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    /* Transfer blocks of data from stdin to shared memory */
    for (xfrs = 0, bytes = 0; /* do nothing */; ++xfrs, bytes += shmp->cnt) {
        /* Wait for our turn */
        if (wait_for_event_flag(semid, WRITE_SEM) == -1) {
            perror("wait_for_event_flag");
            exit(EXIT_FAILURE);
        }
        if (clear_event_flag(semid, WRITE_SEM) == -1) {
            perror("clear_event_flag");
            exit(EXIT_FAILURE);
        }

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* Give reader a turn */
        if (set_event_flag(semid, READ_SEM) == -1) {
            perror("set_event_flag");
            exit(EXIT_FAILURE);
        }

        /* Have we reached EOF? We test this after giving the reader
         * a turn so that it can see the 0 value in shmp->cnt. */
        if (shmp->cnt == 0) {
            break;
        }
    }

    /* Wait until reader has let us have one more turn. We then know
     * reader has finished, and so we can delete the IPC objects */
    if (wait_for_event_flag(semid, WRITE_SEM) == -1) {
        perror("wait_for_event_flag");
        exit(EXIT_FAILURE);
    }
    if (semctl(semid, 0, IPC_RMID, dummy) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
    if (shmdt(shmp) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
