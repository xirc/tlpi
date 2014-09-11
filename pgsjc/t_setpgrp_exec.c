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
#include <signal.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <libgen.h>


int
main(int argc, char *argv[])
{
    char path[PATH_MAX];
    char base[PATH_MAX];
    char *exec_name;
    int before_exec;
    sigset_t waitmask;
    pid_t pid;
    int s, sig;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s path [before-exec]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strncpy(path, argv[1], PATH_MAX);
    path[PATH_MAX - 1] = '\0';
    strncpy(base, argv[1], PATH_MAX);
    base[PATH_MAX - 1] = '\0';
    exec_name = basename(base);

    before_exec = (argc > 2) ? 1 : 0;

    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &waitmask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    /* make stdout unbuffered */
    setbuf(stdout, NULL);

    printf("PARENT: PID=%ld PGID=%ld\n",
            (long) getpid(), (long) getpgrp());

    pid = fork();
    switch (pid) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        /* Child */
        printf("CHILD: PID=%ld PGID=%ld\n",
                (long) getpid(), (long) getpgrp());
        if (before_exec) {
            s = sigwait(&waitmask, &sig);
            if (s != 0) {
                errno = s;
                perror("sigwait");
                exit(EXIT_FAILURE);
            }
        }
        printf("CHILD: PID=%ld PGID=%ld\n",
                (long) getpid(), (long) getpgrp());
        if (execlp(path, path, (char*) NULL) == -1) {
            perror("execlp");
            exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);
        break;
    default:
        /*parent */
        break;
    }

    /* Give child a chance to start */
    sleep(3);

    if (setpgid(pid, pid) == -1) {
        perror("setpgid");
    }

    if (before_exec) {
        if (kill(pid, SIGUSR1) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
    }

    if (wait(NULL) == -1) {
        perror("wait");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
