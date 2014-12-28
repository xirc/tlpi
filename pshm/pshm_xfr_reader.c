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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "pshm_xfr.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    sem_t *sem_reader;
    sem_t *sem_writer;
    int shmfd, xfrs, bytes;
    struct shmseg *shmp;

    sem_reader = sem_open(SEM_READER_NAME, 0);
    if (sem_reader == SEM_FAILED) {
        perror("sem_open - reader");
        exit(EXIT_FAILURE);
    }
    sem_writer = sem_open(SEM_WRITER_NAME, 0);
    if (sem_writer == SEM_FAILED) {
        perror("sem_open - writer");
        exit(EXIT_FAILURE);
    }

    shmfd = shm_open(SHM_NAME, O_RDONLY, 0);
    if (shmfd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    shmp = mmap(NULL, sizeof(struct shmseg),
            PROT_READ, MAP_SHARED, shmfd, 0);
    if (shmp == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /* Transfer blocks of data from shared memory to stdout */
    for (xfrs = 0, bytes = 0; /* do nothing */; xfrs++) {
        /* Wait for our turn */
        if (sem_wait(sem_reader) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }

        /* Writer encounterd EOF */
        if (shmp->cnt == 0) {
            break;
        }
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt) {
            perror("partial/failed write");
            exit(EXIT_FAILURE);
        }

        /* Give writer a turn */
        if (sem_post(sem_writer) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }
    }

    if (munmap(shmp, sizeof(struct shmseg)) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    /* Give writer one more turn, so it can clean up */
    if (sem_post(sem_writer) == -1) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }

    if (sem_close(sem_reader) == -1) {
        perror("sem_close - reader");
        exit(EXIT_FAILURE);
    }
    if (sem_close(sem_writer) == -1) {
        perror("sem_close - writer");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
