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
    sem_t *sem_reader, *sem_writer;
    int shmfd, bytes, xfrs;
    struct shmseg *shmp;

    sem_reader = sem_open(SEM_READER_NAME, O_CREAT | O_EXCL,
            SEM_PERMS, 0);
    if (sem_reader == SEM_FAILED) {
        perror("sem_open - reader");
        exit(EXIT_FAILURE);
    }
    sem_writer = sem_open(SEM_WRITER_NAME, O_CREAT | O_EXCL,
            SEM_PERMS, 1);
    if (sem_writer == SEM_FAILED) {
        perror("sem_open - writer");
        exit(EXIT_FAILURE);
    }

    shmfd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (shmfd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shmfd, sizeof(struct shmseg)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    shmp = mmap(NULL, sizeof(struct shmseg),
            PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (shmp == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /* Transfer blocks of data from stdin to shared memory */
    for (xfrs = 0, bytes = 0;
        /* do nothing */;
        ++xfrs, bytes += shmp->cnt)
    {
        /* Wait for our turn */
        if (sem_wait(sem_writer) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* Give reader a turn */
        if (sem_post(sem_reader) == -1) {
            perror("sem_post");
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
    if (sem_unlink(SEM_READER_NAME) == -1) {
        perror("sem_unlink - reader");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(SEM_WRITER_NAME) == -1) {
        perror("sem_unlink - writer");
        exit(EXIT_FAILURE);
    }
    if (munmap(shmp, sizeof(struct shmseg)) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlik");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
