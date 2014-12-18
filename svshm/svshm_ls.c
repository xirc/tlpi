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
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int maxind, ind, shmid;
    struct shmid_ds ds;
    struct shminfo shminfo;

    /* Obtain size of kernel 'entries' array */
    maxind = shmctl(0, SHM_INFO, (struct shmid_ds *) &shminfo);
    if (maxind == -1) {
        perror("shmctl - SHM_INFO");
        exit(EXIT_FAILURE);
    }

    /* Retrieve and display information
     * from each element of 'entries' array */
    for (ind = 0; ind <= maxind; ++ind) {
        shmid = shmctl(0, SHM_STAT, &ds);
        if (shmid == -1) {
            if (errno != EINVAL && errno != EACCES) {
                perror("msgctl - SHM_STAT");
                exit(EXIT_FAILURE);
            }
            continue;
        }

        printf("%4d %8d  0x%08lx %7ld\n",
                ind, shmid,
                (unsigned long) ds.shm_perm.__key,
                (long) ds.shm_nattch);
    }

    exit(EXIT_FAILURE);
}
