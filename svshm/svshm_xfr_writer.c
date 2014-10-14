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


/*  svshm_xfr_writer.c

   Read buffers of data data from standard input into a System V shared memory
   segment from which it is copied by svshm_xfr_reader.c

   We use a pair of binary semaphores to ensure that the writer and reader have
   exclusive, alternating access to the shared memory. (I.e., the writer writes
   a block of text, then the reader reads, then the writer writes etc). This
   ensures that each block of data is processed in turn by the writer and
   reader.

   This program needs to be started before the reader process as it creates the
   shared memory and semaphores used by both processes.

   Together, these two programs can be used to transfer a stream of data through
   shared memory as follows:

        $ svshm_xfr_writer < infile &
        $ svshm_xfr_reader > out_file
*/


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
    int semid, shmid, bytes, xfrs;
    struct shmseg *shmp;
    union semun dummy;

    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    if (init_sem_available(semid, WRITE_SEM) == -1) {
        perror("init_sem_available");
        exit(EXIT_FAILURE);
    }
    if (init_sem_in_use(semid, READ_SEM) == -1) {
        perror("init_sem_in_use");
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
        if (reserve_sem(semid, WRITE_SEM) == -1) {
            perror("reserve_sem");
            exit(EXIT_FAILURE);
        }

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* Give reader a turn */
        if (release_sem(semid, READ_SEM) == -1) {
            perror("release_sem");
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
    if (reserve_sem(semid, WRITE_SEM) == -1) {
        perror("reserve_sem");
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
