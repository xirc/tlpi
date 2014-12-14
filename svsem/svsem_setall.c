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


/* svsem_setall.c

   Usage: svsem_setall semid value...

   Set the values of all of the members of a System V semaphore set according
   to the values supplied on the command line.
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
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
    union semun arg;
    int j, semid;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s semid val...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    semid = atoi(argv[1]);
    /* Obtain size of semaphore set */
    arg.buf = &ds;
    if (semctl(semid, 0, IPC_STAT, arg) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    if ((int)ds.sem_nsems != argc - 2) {
        fprintf(stderr, "Set contains %ld semaphores, "
                "but %d values were supplied\n",
                (long) ds.sem_nsems, argc - 2);
        exit(EXIT_FAILURE);
    }

    /* Set up array of values; perform semaphore initialization */
    arg.array = calloc(ds.sem_nsems, sizeof(arg.array[0]));
    if (arg.array == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (j = 2; j < argc; ++j) {
        arg.array[j - 2] = atoi(argv[j]);
    }

    if (semctl(semid, 0, SETALL, arg) == -1) {
        perror("semctl - SETALL");
        exit(EXIT_FAILURE);
    }
    printf("Semaphore values changed (PID=%ld)\n", (long) getpid());

    exit(EXIT_SUCCESS);
}
