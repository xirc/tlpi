/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>

#include "binary_sems.h"


int semid;


static void
cleanup(void)
{
    (void) semctl(semid, 0, IPC_RMID);
}


static void
psemval(int semid, int sem_num)
{
    int semval;

    semval = semctl(semid, sem_num, GETVAL);
    if (semval == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
    printf("Semaphore value = %d\n", semval);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    time_t t;

    /* Set cleanup */
    if (atexit(cleanup) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    /* Create semaphore */
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    /* Init semaphore in use */
    printf("init_sem_in_use\n");
    if (init_sem_in_use(semid, 0) == -1) {
        perror("init_sem_in_use");
        exit(EXIT_FAILURE);
    }
    psemval(semid, 0);

    /* Init semaphore available */
    printf("init_sem_available\n");
    if (init_sem_available(semid, 0) == -1) {
        perror("init_sem_available");
        exit(EXIT_FAILURE);
    }
    psemval(semid, 0);

    /* Reserve semaphore */
    printf("reserve_sem\n");
    if (reserve_sem(semid, 0) == -1) {
        perror("reserve_sem");
        exit(EXIT_FAILURE);
    }
    psemval(semid, 0);

    /* Reserve semaphore conditionally */
    printf("reserve_sem_nb expects FAIL\n");
    if (reserve_sem_nb(semid, 0) == -1) {
        perror("reserve_sem_nb");
        /* Do not exit */
    }
    psemval(semid, 0);

    printf("release_sem\n");
    if (release_sem(semid, 0) == -1) {
        perror("release_sem");
        exit(EXIT_FAILURE);
    }
    psemval(semid, 0);

    printf("reserve_sem_nb\n");
    if (reserve_sem_nb(semid, 0) == -1) {
        perror("reserve_sem_nb");
        exit(EXIT_FAILURE);
    }
    psemval(semid, 0);

    printf("SYNC using binary semaphore\n");
    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:     /* Child */
        sleep(3);
        if (release_sem(semid, 0) == -1) {
            perror("reserve_sem");
            exit(EXIT_FAILURE);
        }
        t = time(NULL);
        printf("CHILD: time=%s", ctime(&t));
        exit(EXIT_SUCCESS);
    default:    /* Parent */
        sleep(1);
        if (reserve_sem(semid, 0) == -1) {
            perror("reserve_sem");
            exit(EXIT_FAILURE);
        }
        t = time(NULL);
        printf("PARENT: time=%s", ctime(&t));
        /* fall through */
    }
    if (wait(NULL) == -1) {
        perror("wait");
    }

    exit(EXIT_SUCCESS);
}
