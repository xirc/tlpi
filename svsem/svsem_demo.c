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


/* svsem_demo.c

   A simple demonstration of System V semaphores.
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "semun.h"


#define BUFSIZE 1024
static char *
now(char const *format)
{
    time_t t;
    struct tm tm;
    static char buf[BUFSIZE];

    if (time(&t) == -1) {
        return NULL;
    }

    if (localtime_r(&t, &tm) == NULL) {
        return NULL;
    }

    errno = 0;
    if (strftime(buf, BUFSIZE, format == NULL ? "%T" : format, &tm) == 0 &&
            errno != 0)
    {
        return NULL;
    }

    return buf;
}


int
main(int argc, char *argv[])
{
    int semid;

    if (argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s init-value\n"
                        "  or: %s semid operation\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        /* Create and initialize semaphore */
        union semun arg;
        semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
        if (semid == -1) {
            perror("semget");
            exit(EXIT_FAILURE);
        }

        arg.val = atoi(argv[1]);
        if (semctl(semid, /* semnum= */ 0, SETVAL, arg) == -1) {
            perror("semctl");
            exit(EXIT_FAILURE);
        }

        printf("Semaphore ID = %d\n", semid);
    } else {
        /* Perform an operation on first semaphore */
        struct sembuf sop;
            /* Structure defining operation */
        semid = atoi(argv[1]);
        sop.sem_num = 0;        /* Specifies first semaphore in set */
        sop.sem_op = atoi(argv[2]);
                                /* Add, subtract, or wait for 0 */
        sop.sem_flg = 0;        /* No special options for operation */

        printf("%ld: about to semop at %s\n",
                (long) getpid(), now("%T"));
        if (semop(semid, &sop, 1) == -1) {
            perror("semop");
            exit(EXIT_FAILURE);
        }

        printf("%ld: semop completed at %s\n",
                (long) getpid(), now("%T"));
    }

    exit(EXIT_SUCCESS);
}
