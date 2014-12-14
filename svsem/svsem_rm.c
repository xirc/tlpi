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


/* svsem_rm.c

   Remove the System V semaphore sets whose IDs are specified by the
   command-line arguments.
*/


#include <sys/types.h>
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
    int j;
    union semun dummy;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s [semid...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (j = 1; j < argc; j++) {
        if (semctl(atoi(argv[j]), 0, IPC_RMID, dummy) == -1) {
            fprintf(stderr, "semctl %s: %s\n", argv[j], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
