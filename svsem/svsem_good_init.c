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


/* svsem_good_init.c

   Show how to initialize System V semaphores in a manner that avoids
   race conditions. (Compare with svsem_bad_init.c.)
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
main(int argc, char *argv[])
{
    int semid, key, perms;
    struct sembuf sops[2];

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s sem-op\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    key = 12345;
    perms = S_IRUSR | S_IWUSR;
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | perms);

    if (semid != -1) {
                                /* Successfully created the semaphore */
        union semun arg;
        struct sembuf sop;

        sleep(5);
        printf("%ld: created semaphore\n", (long) getpid());

        arg.val = 0;                    /* So initialize it to 0 */
        if (semctl(semid, 0, SETVAL, arg) == -1) {
            perror("semctl 1");
            exit(EXIT_FAILURE);
        }
        printf("%ld: initialized semaphore\n", (long) getpid());

        /* Perform a "no-op" semaphore operation - changes sem_otime
           so other processes can see we've initialized the set. */
        sop.sem_num = 0;                /* Operate on semaphore 0 */
        sop.sem_op = 0;                 /* Wait for value to equal 0 */
        sop.sem_flg = 0;
        if (semop(semid, &sop, 1) == -1) {
            perror("semop");
            exit(EXIT_FAILURE);
        }
        printf("%ld: completed dummy semop()\n", (long) getpid());
    } else {
                                /* We didn't create the semaphore set */

        if (errno != EEXIST) {          /* Unexpected error from semget() */
            perror("semget 1");
            exit(EXIT_FAILURE);
        } else {                        /* Someone else already created it */
            const int MAX_TRIES = 10;
            int j;
            union semun arg;
            struct semid_ds ds;

            semid = semget(key, 1, perms);      /* So just get ID */
            if (semid == -1) {
                perror("semget 2");
                exit(EXIT_FAILURE);
            }

            printf("%ld: got semaphore key\n", (long) getpid());
            /* Wait until another process has called semop() */

            arg.buf = &ds;
            for (j = 0; j < MAX_TRIES; j++) {
                printf("Try %d\n", j);
                if (semctl(semid, 0, IPC_STAT, arg) == -1) {
                    perror("semctl 2");
                    exit(EXIT_FAILURE);
                }

                if (ds.sem_otime != 0) {        /* Semop() performed? */
                    break;                      /* Yes, quit loop */
                }
                sleep(1);                       /* If not, wait and retry */
            }

            if (ds.sem_otime == 0) {            /* Loop ran to completion! */
                fprintf(stderr, "Existing semaphore not initialized\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    /* Now perform some operation on the semaphore */
    sops[0].sem_num = 0;                       /* Operate on semaphore 0... */
    sops[0].sem_op = atoi(argv[1]);
    sops[0].sem_flg = 0;
    if (semop(semid, sops, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}