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


/* svshm_info.c

   Demonstrate the use of the SHM_INFO operation to retrieve a 'shminfo'
   structure containing the current usage of System V shared memory resources.

   This program is Linux-specific.
*/


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
    struct shm_info info;
    int s;

    s = shmctl(0, SHM_INFO, (struct shmid_ds *) &info);
    if (s == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    printf("Maximum ID index = %d\n", s);
    printf("shm_tot          = %ld\n", (long) info.shm_tot);
    printf("shm_rss          = %ld\n", (long) info.shm_rss);
    printf("shm_swp          = %ld\n", (long) info.shm_swp);
    printf("swap_attempts    = %ld\n", (long) info.swap_attempts);
    printf("swap_successes   = %ld\n", (long) info.swap_successes);

    exit(EXIT_SUCCESS);
}
