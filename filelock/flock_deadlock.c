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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/file.h>
#include <signal.h>


char *lockfile1 = "flock_deadlock.1";
char *lockfile2 = "flock_deadlock.1";


void
cleanup(int sig __attribute__((unused)))
{
    (void) unlink(lockfile1);
    (void) unlink(lockfile2);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int cpid;
    int fd1, fd2;
    struct sigaction sa;

    printf("%s will be in deadlock state, or detects deadlock.\n", argv[0]);
    printf("It is depends on UNIX/LINUX OS.\n");

    fd1 = open(lockfile1, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd1 == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    (void) close(fd1);

    fd2 = open(lockfile2, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd2 == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    (void) close(fd2);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = cleanup;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction SIGINT");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction SIGINT");
        exit(EXIT_FAILURE);
    }

    switch (cpid = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    default:
        break;
    }

    fd1 = open(lockfile1, O_RDWR);
    if (fd1 == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    fd2 = open(lockfile2, O_RDWR);
    if (fd2 == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    switch (cpid) {
    case 0:  /* CHILD */
        puts("CHILD: flock 1");
        if (flock(fd1, LOCK_EX) == -1) {
            perror("CHILD: flock 1");
            exit(EXIT_FAILURE);
        }
        sleep(1);
        puts("CHILD: flock 2");
        if (flock(fd2, LOCK_EX) == -1) {
            perror("CHILD: flock 2");
            exit(EXIT_FAILURE);
        }
        (void) flock(fd2, LOCK_UN);
        (void) flock(fd1, LOCK_UN);
        break;
    default: /* PARENT */
        puts("PARENT: flock 2");
        if (flock(fd2, LOCK_EX) == -1) {
            perror("PARENT: flock 2");
            exit(EXIT_FAILURE);
        }
        sleep(1);
        puts("PARENT: flock 1");
        if (flock(fd1, LOCK_EX) == -1) {
            perror("PARENT: flock 1");
            exit(EXIT_FAILURE);
        }
        (void) flock(fd1, LOCK_UN);
        (void) flock(fd2, LOCK_UN);
        break;
    }

    (void) unlink(lockfile1);
    (void) unlink(lockfile2);
    (void) close(fd1);
    (void) close(fd2);
    exit(EXIT_SUCCESS);
}
