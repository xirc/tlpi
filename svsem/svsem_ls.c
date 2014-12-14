/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int maxind, ind, semid;
    struct seminfo seminfo;
    struct semid_ds ds;

    /* Obtain size of kernel 'entries' array */
    maxind = semctl(0, 0, SEM_INFO, (struct semid_ds *) &seminfo);
    if (maxind == -1) {
        perror("semctl SEM_INFO");
        exit(EXIT_FAILURE);
    }

    printf("maxind: %d\n\n", maxind);
    printf("index    id       key      message\n");
    /* Retrieve and display information from
     * each element of 'entries' array */
    for (ind = 0; ind <= maxind; ++ind) {
        semid = semctl(ind, 0, SEM_STAT, &ds);
        if (semid == -1) {
            if (errno != EINVAL && errno != EACCES) {
                perror("semctl SEM_STAT");
            }
            continue;
        }

        printf("%4d %8d  0x%08lx %7ld\n",
                ind, semid, (unsigned long) ds.sem_perm.__key, ds.sem_nsems);
    }

    exit(EXIT_SUCCESS);
}
