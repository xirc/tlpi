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
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "semun.h"
#include "mmap_xfr.h"
#include "binary_sems.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int semid, bytes, xfrs;
    union semun dummy;
    int fd, retc;
    void *mmapaddr;
    struct mmapseg *mapsegp;

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

    /* Make shared memory file mapping */
    fd = open(SHARED_MEMORY_MAPPING_FILE, O_RDWR | O_CREAT);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    retc = ftruncate(fd, sizeof(struct mmapseg));
    if (retc == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    mmapaddr = mmap(NULL, sizeof(struct mmapseg),
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmapaddr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    mapsegp = (struct mmapseg *) mmapaddr;

    /* Transfer blocks of data from stdin to shared memory */
    for (xfrs = 0, bytes = 0; /* do nothing */; ++xfrs, bytes += mapsegp->cnt) {
        /* Wait for our turn */
        if (reserve_sem(semid, WRITE_SEM) == -1) {
            perror("reserve_sem");
            exit(EXIT_FAILURE);
        }

        mapsegp->cnt = read(STDIN_FILENO, mapsegp->buf, BUF_SIZE);
        if (mapsegp->cnt == -1) {
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
        if (mapsegp->cnt == 0) {
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

    /* Remove shared memory file mapping */
    retc = munmap(mmapaddr, sizeof(struct mmapseg));
    if (retc == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    retc = close(fd);
    if (retc == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    retc = unlink(SHARED_MEMORY_MAPPING_FILE);
    if (retc == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
