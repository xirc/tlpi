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


/* svsem_mon.c

   Display various information about the semaphores in a System V semaphore set.

   Since the information obtained by this program is not obtained atomically,
   it may not be consistent if another process makes changes to the semaphore
   at the moment this program is running.
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "semun.h"


int
main(int argc, char *argv[])
{
    struct semid_ds ds;
    union semun arg, dummy;
    int semid, j;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s semid\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    semid = atoi(argv[1]);
    arg.buf = &ds;
    if (semctl(semid, 0, IPC_STAT, arg) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    printf("Semaphore changed:  %s", ctime(&ds.sem_ctime));
    printf("Last semop():       %s", ctime(&ds.sem_otime));

    /* Display per-semaphore information */
    arg.array = calloc(ds.sem_nsems, sizeof(arg.array[0]));
    if (arg.array == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    if (semctl(semid, 0, GETALL, arg) == -1) {
        perror("semctl - GETALL");
        exit(EXIT_FAILURE);
    }

    printf("Sem #  Value  SEMPID  SEMNCNT  SEMZCNT\n");
    for (j = 0; j < (int)ds.sem_nsems; ++j) {
        printf("%3d   %5d   %5d  %5d    %5d\n", j, arg.array[j],
                semctl(semid, j, GETPID, dummy),
                semctl(semid, j, GETNCNT, dummy),
                semctl(semid, j, GETZCNT, dummy));
    }

    exit(EXIT_SUCCESS);
}
