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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>


int
main(int argc, char *argv[])
{
    sem_t *sem;
    int use_timedwait = 0;
    unsigned long timeout_s;
    struct timespec ts;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s sem-name [timeout]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argc == 3) {
        use_timedwait = 1;
        timeout_s = strtoul(argv[2], NULL, 0);
    }

    sem = sem_open(argv[1], 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if (use_timedwait) {
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }
        ts.tv_sec += timeout_s;
        if (sem_timedwait(sem, &ts) == -1) {
            perror("sem_timedwait");
            exit(EXIT_FAILURE);
        }
    } else {
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
    }

    printf("%ld %s() succeeded\n", (long) getpid(),
            use_timedwait ? "sem_timedwait" : "sem_wait");

    exit(EXIT_SUCCESS);
}
