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
#include <fcntl.h>
#include <sys/wait.h>

#include "region_locking.h"


static int lockfd;


static void
rdlock(int start, int len, int is_wait)
{
    int retc;
    if (is_wait) {
        retc = lock_region_wait(lockfd, F_RDLCK, SEEK_SET, start, len);
    } else {
        retc = lock_region(lockfd, F_RDLCK, SEEK_SET, start, len);
    }
    if (retc == -1) {
        printf("%ld: RDLCK (FAIL %s)\n", (long) getpid(), strerror(errno));
    } else {
        printf("%ld: RDLCK\n", (long) getpid());
    }
}


static void
wrlock(int start, int len, int is_wait)
{
    int retc;
    if (is_wait) {
        retc = lock_region_wait(lockfd, F_WRLCK, SEEK_SET, start, len);
    } else {
        retc = lock_region(lockfd, F_WRLCK, SEEK_SET, start, len);
    }
    if (retc == -1) {
        printf("%ld: WRLCK (FAIL %s)\n", (long) getpid(), strerror(errno));
    } else {
        printf("%ld: WRLCK\n", (long) getpid());
    }
}


static void
unlock(int start, int len)
{
    int retc;

    retc = lock_region(lockfd, F_UNLCK, SEEK_SET, start, len);
    if (retc == -1) {
        perror("lock_region - F_UNLCK");
    } else {
        printf("%ld: UNLOCK\n", (long) getpid());
    }
}

static void
islock()
{
    pid_t pid;
    pid = is_region_locked(lockfd, F_WRLCK, SEEK_SET, 0, 0);
    if (pid == -1) {
        perror("is_region_locked");
    } else {
        printf("is_region_locked: %ld\n", (long) pid);
    }
}


int
main(int argc, char *argv[])
{
    pid_t pid;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lockfd = open(argv[1], O_RDWR);
    if (lockfd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    switch (pid = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:     /* CHILD */
        sleep(1);
        islock();
        rdlock(0, 0, 0);    /* FAIL */
        rdlock(0, 0, 1);
        sleep(1);
        unlock(0, 0);
        wrlock(0, 0, 1);
        sleep(2);
        unlock(0, 0);
        exit(EXIT_SUCCESS);
    default:    /* PARENT */
        wrlock(0, 0, 0);
        sleep(2);
        unlock(0, 0);
        rdlock(0, 0, 0);
        sleep(2);
        unlock(0, 0);
        sleep(1);
        wrlock(0, 0, 0);
        break;
    }

    (void) wait(NULL);
    exit(EXIT_SUCCESS);
}
