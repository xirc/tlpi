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
#include "binary_sems.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int semid, shmid, xfrs, bytes;
    struct shmseg *shmp;

    /* Get IDs for semaphore set and shared memory created by writer */
    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    shmid = shmget(SHM_KEY, 0, 0);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shmp = shmat(shmid, NULL, SHM_RDONLY);
    if (shmp == (void*) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    /* Transfer blocks of data from shared memory to stdout */
    for (xfrs = 0, bytes = 0; shmp->cnt != 0; xfrs++, bytes += shmp->cnt) {
        /* Wait for our turn */
        if (reserve_sem(semid, READ_SEM) == -1) {
            perror("reserve_sem");
            exit(EXIT_FAILURE);
        }

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt) {
            perror("partial/failed write");
            exit(EXIT_FAILURE);
        }

        /* Give writer a turn */
        if (release_sem(semid, WRITE_SEM) == -1) {
            perror("release_sem");
            exit(EXIT_FAILURE);
        }
    }

    if (shmdt(shmp) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    /* Give writer one more turn, so it can clean up */
    if (release_sem(semid, WRITE_SEM) == -1) {
        perror("release_sem");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
