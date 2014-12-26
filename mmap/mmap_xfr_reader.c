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
    int semid, xfrs, bytes;
    int fd, retc;
    struct mmapseg *mapsegp;
    void *mmapaddr;

    /* Get IDs for semaphore set and shared memory created by writer */
    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    /* Make shared memory file mapping */
    fd = open(SHARED_MEMORY_MAPPING_FILE, O_RDWR);
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

    /* Transfer blocks of data from shared memory to stdout */
    for (xfrs = 0, bytes = 0; /* do nothing */; xfrs++) {
        /* Wait for our turn */
        if (reserve_sem(semid, READ_SEM) == -1) {
            perror("reserve_sem");
            exit(EXIT_FAILURE);
        }

        /* Writer encounterd EOF */
        if (mapsegp->cnt == 0) {
            break;
        }
        bytes += mapsegp->cnt;

        if (write(STDOUT_FILENO, mapsegp->buf, mapsegp->cnt) !=
                mapsegp->cnt)
        {
            perror("partial/failed write");
            exit(EXIT_FAILURE);
        }

        /* Give writer a turn */
        if (release_sem(semid, WRITE_SEM) == -1) {
            perror("release_sem");
            exit(EXIT_FAILURE);
        }
    }

    /* Give writer one more turn, so it can clean up */
    if (release_sem(semid, WRITE_SEM) == -1) {
        perror("release_sem");
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

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
