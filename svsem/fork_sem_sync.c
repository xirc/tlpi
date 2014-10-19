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
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#include "semun.h"
#include "binary_sems.h"


static char * now(char const *format);


static int semid;


static void
cleanup(void)
{
    (void) semctl(semid, 0, IPC_RMID);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pid_t cpid;
    union semun arg;
    struct sembuf sops[1];

    setbuf(stdout, NULL);
        /* Disable buffering of stdout */

    /* Create Semaphore and set it's cleanup */
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    if (atexit(cleanup) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }
    /* Set semaphore which is unavailable */
    arg.val = 0;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    switch (cpid = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        /* Child does some required action here... */
        printf("[%s %ld] Child started - doing some work\n",
                now("%T"), (long) getpid());
        sleep(2);
            /* Simulate time spent doing some work */
        /* And then child release semaphore */
        printf("[%s %ld] Child about to SEM_RELEASE\n",
                now("%T"), (long) getpid());

        /* Release semaphore */
        sops[0].sem_num = 0;
        sops[0].sem_op = +1;
        sops[0].sem_flg = 0;
        if (semop(semid, sops, 1) == -1) {
            perror("semop");
            exit(EXIT_FAILURE);
        }

        /* Now child can do other things... */
        _exit(EXIT_FAILURE);

    default:
        /* Parent may do some work here,
         * and then waits for child to complete the required action */
        printf("[%s %ld] Parent about to wait for SEM_RELEASE\n",
                now("%T"), (long) getpid());

        /* Acquire semaphore */
        sops[0].sem_num = 0;
        sops[0].sem_op = -1;
        sops[0].sem_flg = 0;
        if (semop(semid, sops, 1) == -1) {
            perror("semop");
            exit(EXIT_FAILURE);
        }
        printf("[%s %ld] Parent aquired semaphore\n",
                now("%T"), (long) getpid());

        /* Parent carries on to do other things... */

        exit(EXIT_SUCCESS);
    }
}


#define BUF_SIZE 1024
static char *
now(char const *format)
{
    static char buf[BUF_SIZE];
    time_t t;
    size_t s;
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);
    if (tm == NULL) {
        return NULL;
    }
    s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);
    return (s == 0) ? NULL : buf;
}
