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

#include "event_flag.h"


static int semid;


static void
cleanup(void)
{
    (void) semctl(semid, 0, IPC_RMID);
}


static char*
strflag(int event_flag)
{
    if (event_flag == FLAG_SET) {
        return "SET";
    } else if (event_flag == FLAG_CLEAR) {
        return "CLEAR";
    }
    return "?";
}


static void
pflag(int semid, int sem_num)
{
    int flagval;

    flagval = get_flag_state(semid, sem_num);
    if (flagval == -1) {
        perror("get_flag_state");
        exit(EXIT_FAILURE);
    }
    printf("Event flag = %s\n", strflag(flagval));
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

    /* Init event flag */
    printf("init_event_flag\n");
    if (init_event_flag(semid, 0) == -1) {
        perror("init_event_flag");
        exit(EXIT_FAILURE);
    }
    pflag(semid, 0);

    /* Set event flag */
    printf("set_event_flag\n");
    if (set_event_flag(semid, 0) == -1) {
        perror("set_event_flag");
        exit(EXIT_FAILURE);
    }
    pflag(semid, 0);

    /* Clear event flag */
    printf("clear_event_flag\n");
    if (clear_event_flag(semid, 0) == -1) {
        perror("clear_event_flag");
        exit(EXIT_FAILURE);
    }
    pflag(semid, 0);

    printf("wait_for_event_flag\n");
    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:     /* Child */
        sleep(3);
        if (set_event_flag(semid, 0) == -1) {
            perror("set_event_flag");
            exit(EXIT_FAILURE);
        }
        t = time(NULL);
        printf("CHILD: time=%s", ctime(&t));
        exit(EXIT_SUCCESS);
    default:    /* Parent */
        sleep(1);
        if (wait_for_event_flag(semid, 0) == -1) {
            perror("wait_for_event_flag");
            exit(EXIT_FAILURE);
        }
        t = time(NULL);
        printf("PARENT: time=%s", ctime(&t));
        if (clear_event_flag(semid, 0) == -1) {
            perror("clear_event_flag");
            exit(EXIT_FAILURE);
        }
        /* fall through */
    }
    if (wait(NULL) == -1) {
        perror("wait");
    }

    exit(EXIT_SUCCESS);
}
