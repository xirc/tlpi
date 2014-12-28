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


/* psem_getvalue.c

   Obtain the value of a POSIX named semaphore.

   On Linux, named semaphores are supported with kernel 2.6 or later, and
   a glibc that provides the NPTL threading implementation.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>


int
main(int argc, char *argv[])
{
    int value;
    sem_t *sem;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s sem-name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sem = sem_open(argv[1], 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if (sem_getvalue(sem, &value) == -1) {
        perror("sem_getvalue");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", value);

    exit(EXIT_SUCCESS);
}
