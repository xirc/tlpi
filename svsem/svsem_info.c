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


/* svsem_info.c

   Demonstrate the use of the SEM_INFO operation to retrieve a 'seminfo'
   structure containing the current usage of System V semaphore resources.

   This program is Linux-specific.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "semun.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct seminfo info;
    union semun arg;
    int s;

    arg.__buf = &info;
    s = semctl(0, 0, SEM_INFO, arg);
    if (s == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    printf("Maximum ID index = %d\n", s);
    printf("sets in_use      = %ld\n", (long) info.semusz);
    printf("used_sems        = %ld\n", (long) info.semaem);
    exit(EXIT_SUCCESS);
}
