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


/* svsem_bad_init.c

   A demonstration of the wrong way to initialize a system V semaphore.

   Compare this program with svsem_good_init.c.
*/


#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "semun.h"


int
main(int argc __attribute__((unused)),
    char *argv[] __attribute__((unused)))
{
    int semid, key, perms;
    struct sembuf sops[2];

    key = 12345;
    perms = S_IRUSR | S_IWUSR;
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | perms);

    if (semid != -1) {
                             /* Successfully created the semaphore */
        union semun arg;

        /* XXXX */

        arg.val = 0;                    /* So initialize it */
        if (semctl(semid, 0, SETVAL, arg) == -1) {
            perror("semctl");
            exit(EXIT_FAILURE);
        }
    } else {                            /* We didn't create semaphore set */
        if (errno != EEXIST) {          /* Unexpected error from semget() */
            perror("semget 1");
            exit(EXIT_FAILURE);
        } else {                        /* Someone else already created it */
            semid = semget(key, 1, perms);      /* So just get ID */
            if (semid == -1) {
                perror("semget 2");
                exit(EXIT_FAILURE);
            }
        }
    }

    /* Now perform some operation on the semaphore */
    sops[0].sem_op = 1;         /* Add 1 */
    sops[0].sem_num = 0;        /* ... to semaphore 0 */
    sops[0].sem_flg = 0;
    if (semop(semid, sops, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
